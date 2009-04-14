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
#include "elpea-thumbnail-view.h"
#include "elpea-thumbnail.h"


G_DEFINE_TYPE (ElpeaThumbnailView, elpea_thumbnail_view, GTK_TYPE_TREE_VIEW)



static void
cell_data_func                                          (GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);




struct _ElpeaThumbnailViewPrivate
{
	/* Private members go here */

	gboolean disposed;
};


#define ELPEA_THUMBNAIL_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	ELPEA_TYPE_THUMBNAIL_VIEW, ElpeaThumbnailViewPrivate))


GtkWidget*
elpea_thumbnail_view_new (void)
{
	ElpeaThumbnailView *self = g_object_new (ELPEA_TYPE_THUMBNAIL_VIEW, NULL);
	return self;
}


static void
elpea_thumbnail_view_init_gui (ElpeaThumbnailView *self)
{
	g_print ("elpea_thumbnail_view_init_gui\n");
	ElpeaThumbnailViewPrivate *priv = self->priv;

	/* Set up column */
	GtkTreeViewColumn *column = gtk_tree_view_column_new ();
	gtk_tree_view_column_set_title (column, "Images");

	/* Create renderers */
	GtkCellRenderer *pix = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (column, pix, FALSE);
	gtk_tree_view_column_add_attribute (column, pix, "pixbuf", 1);


	GtkCellRenderer *txt = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, txt, TRUE);
	gtk_tree_view_column_set_cell_data_func (column, txt, cell_data_func, self, NULL);
//	gtk_tree_view_column_add_attribute (column, txt, "markup", 1);

	/* Misc */
	gtk_tree_view_append_column (GTK_TREE_VIEW (self), column);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (self), FALSE);
}


static void
cell_data_func                                          (GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
	ElpeaThumbnail *thumb;
	gtk_tree_model_get (tree_model, iter, 0, &thumb, -1);

	const gchar *file = elpea_thumbnail_get_name (thumb);
	gint size = elpea_thumbnail_get_size (thumb);
	gint width = elpea_thumbnail_get_width (thumb);
	gint height = elpea_thumbnail_get_height (thumb);

	//gchar *txt = g_strdup_printf ("<b>%s</b>\n<small>%dx%d pixels\n%d bytes</small>", file, width, height, size);
	gchar *txt = g_strdup_printf ("<b>%s</b>\n<small>%d\342\250\257%d pixels\n%d bytes</small>", file, width, height, size);
	g_object_set (G_OBJECT (cell), "markup", txt, NULL);
	g_free (txt);

	g_object_unref (thumb);
}


static void
elpea_thumbnail_view_init (ElpeaThumbnailView *self)
{
	self->priv = ELPEA_THUMBNAIL_VIEW_GET_PRIVATE (self);
	ElpeaThumbnailViewPrivate *priv = self->priv;

	priv->disposed = FALSE;

	elpea_thumbnail_view_init_gui (self);
}


static void
elpea_thumbnail_view_dispose (GObject *object)
{
	ElpeaThumbnailView *self = (ElpeaThumbnailView*) object;
	ElpeaThumbnailViewPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (elpea_thumbnail_view_parent_class)->dispose (object);
}


static void
elpea_thumbnail_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (elpea_thumbnail_view_parent_class)->finalize (object);
}

	
static void
elpea_thumbnail_view_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ElpeaThumbnailView* self = ELPEA_THUMBNAIL_VIEW (object);
	ElpeaThumbnailViewPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_thumbnail_view_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ElpeaThumbnailView* self = ELPEA_THUMBNAIL_VIEW (object);
	ElpeaThumbnailViewPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_thumbnail_view_class_init (ElpeaThumbnailViewClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = elpea_thumbnail_view_get_property;
	gobject_class->set_property = elpea_thumbnail_view_set_property;
	gobject_class->dispose = elpea_thumbnail_view_dispose;
	gobject_class->finalize = elpea_thumbnail_view_finalize;

	g_type_class_add_private (klass, sizeof (ElpeaThumbnailViewPrivate));
}

