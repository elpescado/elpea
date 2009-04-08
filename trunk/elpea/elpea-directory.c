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

#include <gtk/gtk.h>

#include "elpea-directory.h"
#include "elpea-thumbnail.h"

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

	gboolean disposed;
};


#define ELPEA_DIRECTORY_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	ELPEA_TYPE_DIRECTORY, ElpeaDirectoryPrivate))

#define N_COLUMNS 2

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

	priv->disposed = FALSE;
}


/* Directory functions */


void
elpea_directory_load (ElpeaDirectory *self,
                      const gchar *dirname)
{
	g_return_if_fail (ELPEA_IS_DIRECTORY (self));


	GtkListStore *store = GTK_LIST_STORE (self);
	gtk_list_store_clear (store);

	GDir *dir = g_dir_open (dirname, 0, NULL);
	if (dir == NULL)
		return;

	const gchar *file;
	while ((file = g_dir_read_name (dir))) {
		g_print ("Trying to load %s... ", file);
		
		ElpeaThumbnail *thumb = elpea_thumbnail_new (dirname, file);
		if (thumb) {
			GtkTreeIter iter;
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, thumb, -1);
			g_print ("ok\n");
		} else {
			g_print ("failed\n");
		}
	}
	g_dir_close (dir);
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

