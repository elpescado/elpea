/*
 * elpea
 * -----
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include <gtk/gtk.h>

#include "elpea-directory.h"
#include "elpea-thumbnail.h"

#include "ooze-stree.h"

static void
elpea_directory_tree_model_iface_init (GtkTreeModelIface *iface);

G_DEFINE_TYPE_EXTENDED (ElpeaDirectory, elpea_directory, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, elpea_directory_tree_model_iface_init));
//G_DEFINE_TYPE (ElpeaDirectory, elpea_directory, GTK_TYPE_LIST_STORE)
/* our parent's model iface */
static GtkTreeModelIface parent_iface = { 0 };


struct _ElpeaDirectoryPrivate
{
	/* Private members go here */

	GMutex *mutex;

	gint n_items;


	gboolean disposed;
};

typedef struct {
	ElpeaDirectory *directory;
	int i;
} NotifyData;



#define ELPEA_DIRECTORY_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	ELPEA_TYPE_DIRECTORY, ElpeaDirectoryPrivate))

#define N_COLUMNS 2



static OozeSTree *extensions;


ElpeaDirectory*
elpea_directory_new (void)
{
	ElpeaDirectory *self = g_object_new (ELPEA_TYPE_DIRECTORY, NULL);

	return self;
}


static void
elpea_directory_init (ElpeaDirectory *self)
{
	self->priv = ELPEA_DIRECTORY_GET_PRIVATE (self);
	ElpeaDirectoryPrivate *priv = self->priv;

	GType types[] = { ELPEA_TYPE_THUMBNAIL };
	gtk_list_store_set_column_types (GTK_LIST_STORE (self), 1, types);

	priv->mutex = g_mutex_new ();

	priv->disposed = FALSE;
}


/* Directory functions */

static void
load_extensions (void)
{
	extensions = ooze_stree_new ();

	g_print ("Supported extensions:\n");
	GSList *formats = gdk_pixbuf_get_formats ();
	for (;formats; formats = formats->next) {
		GdkPixbufFormat *format = (GdkPixbufFormat *) formats->data;

		char **exts = gdk_pixbuf_format_get_extensions (format);
		int i;

		for (i = 0; exts[i]; i++) {
			char extbuf[32];
			strncpy (extbuf+1, exts[i], 30);
			extbuf[0] = '.';
			extbuf[31] = '\0';

			ooze_stree_add (extensions, extbuf, -1);
			g_print (" * %s\n", extbuf);
			//g_print (" * %s\n", exts[i]);
		}
		
		g_strfreev (exts);
	}
	g_slist_free (formats);
}


gboolean
file_filter (const gchar *dirname, const gchar *name)
{
	/* Discard hidden files */
	if (name[0] == '.')
		return FALSE;

	/* Accept only regular files and symlinks to regular files */
	char *path = g_build_filename (dirname, name, NULL);
	if (! g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
		g_free (path);
		return FALSE;
	}
	g_free (path);

	/* Check extensions */
	/*
	if (strstr (name, ".jpg") == NULL) // FIXME
		return FALSE;
	*/
	return ooze_stree_ends_with (extensions, name, -1);
	
//	return TRUE;
}

static int
elpea_directory_get_n_items (ElpeaDirectory *self)
{
	ElpeaDirectoryPrivate *priv = self->priv;

	g_mutex_lock (priv->mutex);
	int i = priv->n_items;
	g_mutex_unlock (priv->mutex);
	return i;
}

/* Thread loader - very dirty */

static gboolean
_thumbnailer_notify (gpointer user_data)
{
	NotifyData *data = user_data;
	g_print ("_thumbnailer_notify (%d)\n", data->i);

	ElpeaDirectory *self = ELPEA_DIRECTORY (data->directory);
	GtkTreePath *path = gtk_tree_path_new_from_indices (data->i, -1);

	GtkTreeIter iter;

	if (! gtk_tree_model_get_iter (GTK_TREE_MODEL (self), &iter, path)) {
		g_print ("gtk_tree_model_get_iter failed:-/\n");
		return FALSE;
	}

	ElpeaThumbnail *thumb;
	gtk_tree_model_get (GTK_TREE_MODEL (self), &iter, 0, &thumb, -1);
	gtk_list_store_set (GTK_LIST_STORE (self), &iter, 0, thumb, -1);

//	gtk_tree_model_row_changed (GTK_TREE_MODEL (self), &iter, path);

	return FALSE;
}


static gpointer
_t_thumbnailer_thread (gpointer user_data)
{
	ElpeaDirectory *self = ELPEA_DIRECTORY (user_data);
	ElpeaDirectoryPrivate *priv = self->priv;
	GtkListStore *store = GTK_LIST_STORE (self);

	int n_items = elpea_directory_get_n_items (self);
	int i;
	
	for (i = 0; i < n_items; i++) {
		/* TODO: Try to use iterators. It will probably fail,
		 * as iterators are immutable, but it may be worth trying */
		GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);

		GtkTreeIter iter;
		ElpeaThumbnail *thumb;

		gtk_tree_model_get_iter (GTK_TREE_MODEL (self), &iter, path);
		gtk_tree_model_get (GTK_TREE_MODEL (self), &iter, 0, &thumb, -1);

		/* Load thumbnail */
		elpea_thumbnail_load (thumb);

		/* Post a notify */
		NotifyData *notify_data = g_new (NotifyData, 1);
		notify_data->directory = self;
		notify_data->i = i;
		g_timeout_add_full (G_PRIORITY_DEFAULT, 1,
		                    _thumbnailer_notify,
							notify_data,
							g_free);


		gtk_tree_path_free (path);
		g_usleep (1000*1000);
	}
}


static void
start_thumbnailer_thread (ElpeaDirectory *self)
{
	ElpeaDirectoryPrivate *priv = self->priv;
	GtkListStore *store = GTK_LIST_STORE (self);

	GThread *thread = g_thread_create (_t_thumbnailer_thread,
	                                   self,
									   FALSE,
									   NULL);
}


void
elpea_directory_load (ElpeaDirectory *self,
                      const gchar *dirname)
{
	g_return_if_fail (ELPEA_IS_DIRECTORY (self));

	// FIXME
	load_extensions ();

	ElpeaDirectoryPrivate *priv = self->priv;
	GtkListStore *store = GTK_LIST_STORE (self);
	gtk_list_store_clear (store);

	GDir *dir = g_dir_open (dirname, 0, NULL);
	if (dir == NULL)
		return;

	gint n = 0;

	const gchar *file;
	while ((file = g_dir_read_name (dir))) {
		if (! file_filter (dirname, file))
				continue;
		g_print ("Trying to load %s... ", file);
		
		ElpeaThumbnail *thumb = elpea_thumbnail_new (dirname, file);
		if (thumb) {
			GtkTreeIter iter;
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, thumb, -1);
			g_print ("ok\n");
			n++;
		} else {
			g_print ("failed\n");
		}
	}
	g_dir_close (dir);
	priv->n_items = n;

	start_thumbnailer_thread (self);
}


/* GtkTreeModel implementation */

static int
elpea_directory_get_n_columns (GtkTreeModel *self)
{
        /* validate our parameters */
        g_return_val_if_fail (ELPEA_IS_DIRECTORY (self), 0);

        return N_COLUMNS;
}

static GType
elpea_directory_get_column_type (GtkTreeModel *self, int column)
{
        GType types[] = {
                ELPEA_TYPE_THUMBNAIL,
				GDK_TYPE_PIXBUF
        };

        /* validate our parameters */
        g_return_val_if_fail (ELPEA_IS_DIRECTORY (self), G_TYPE_INVALID);
        g_return_val_if_fail (column >= 0 && column < N_COLUMNS, G_TYPE_INVALID);

        return types[column];
}

static ElpeaThumbnail *
elpea_directory_get_object (ElpeaDirectory *self, GtkTreeIter *iter)
{
        GValue value = { 0, };
        ElpeaThumbnail *obj;

        /* validate our parameters */
        g_return_val_if_fail (ELPEA_IS_DIRECTORY (self), NULL);
        g_return_val_if_fail (iter != NULL, NULL);

        /* retreive the object using our parent's interface, take our own
         * reference to it */
        parent_iface.get_value (GTK_TREE_MODEL (self), iter, 0, &value);
        obj = ELPEA_THUMBNAIL (g_value_dup_object (&value));

        g_value_unset (&value);

        return obj;
}


static void
elpea_directory_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column,
                GValue *value)
{
        ElpeaThumbnail *obj;

        /* validate our parameters */
        g_return_if_fail (ELPEA_IS_DIRECTORY (self));
        g_return_if_fail (iter != NULL);
        g_return_if_fail (column >= 0 && column < N_COLUMNS);
        g_return_if_fail (value != NULL);

        /* get the object from our parent's storage */
        obj = elpea_directory_get_object (ELPEA_DIRECTORY (self), iter);

        /* initialise our GValue to the required type */
        g_value_init (value,
                elpea_directory_get_column_type (GTK_TREE_MODEL (self), column));


		GdkPixbuf *pix;
        switch (column)
        {
                case 0:
                        g_value_set_object (value, obj);
                        break;

                case 1:
						pix = elpea_thumbnail_get_pixbuf (obj);
                        g_value_set_object (value, pix);
                        break;

                default:
                        g_assert_not_reached ();
        }

        /* release the reference gained from elpea_directory_get_object() */
        g_object_unref (obj);
}

/* GObject stuff */

static void
elpea_directory_dispose (GObject *object)
{
	ElpeaDirectory *self = (ElpeaDirectory*) object;
	ElpeaDirectoryPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (elpea_directory_parent_class)->dispose (object);
}


static void
elpea_directory_finalize (GObject *object)
{
	G_OBJECT_CLASS (elpea_directory_parent_class)->finalize (object);
}

	
static void
elpea_directory_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ElpeaDirectory* self = ELPEA_DIRECTORY (object);
	ElpeaDirectoryPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_directory_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ElpeaDirectory* self = ELPEA_DIRECTORY (object);
	ElpeaDirectoryPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
elpea_directory_tree_model_iface_init (GtkTreeModelIface *iface)
{
        /* this is where we override the interface methods */
        /* first make a copy of our parent's interface to call later */
        parent_iface = *iface;

        /* now put in our own overriding methods */
        iface->get_n_columns = elpea_directory_get_n_columns;
        iface->get_column_type = elpea_directory_get_column_type;
        iface->get_value = elpea_directory_get_value;
}


static void
elpea_directory_class_init (ElpeaDirectoryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = elpea_directory_get_property;
	gobject_class->set_property = elpea_directory_set_property;
	gobject_class->dispose = elpea_directory_dispose;
	gobject_class->finalize = elpea_directory_finalize;

	g_type_class_add_private (klass, sizeof (ElpeaDirectoryPrivate));
}

