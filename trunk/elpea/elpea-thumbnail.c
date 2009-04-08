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
#include "elpea-thumbnail.h"



static void
elpea_thumbnail_init (ElpeaThumbnail *self);

static GdkPixbuf *
elpea_thumbnail_scale (GdkPixbuf *pix);



G_DEFINE_TYPE (ElpeaThumbnail, elpea_thumbnail, G_TYPE_OBJECT)

struct _ElpeaThumbnailPrivate
{
	/* Private members go here */
	GdkPixbuf   *thumbnail;

	gchar       *name;		/**< File name             */
	gchar       *path;      /**< Full file path        */
	gint         width;		/**< Original image width  */
	gint         height;	/**< Original image height */
	gsize        size;		/**< Original image size   */

	gboolean disposed;
};


#define ELPEA_THUMBNAIL_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	ELPEA_TYPE_THUMBNAIL, ElpeaThumbnailPrivate))


ElpeaThumbnail*
elpea_thumbnail_new (const gchar *dirname, const gchar *file)
{
	/* Elpea */
	char *path = g_build_filename (dirname, file, NULL);
	GdkPixbuf *pix = gdk_pixbuf_new_from_file (path, NULL);
	if (pix == NULL) {
		g_free (path);
		return NULL;
	}

	/* GObject */
	ElpeaThumbnail *self = g_object_new (ELPEA_TYPE_THUMBNAIL, NULL);
	ElpeaThumbnailPrivate *priv = self->priv;

	priv->thumbnail = elpea_thumbnail_scale (pix);
	priv->name   = g_strdup (file);
	priv->path   = path;
	priv->width  = gdk_pixbuf_get_width (pix);
	priv->height = gdk_pixbuf_get_height (pix);
	priv->size   = get_file_size (path);

	g_object_unref (G_OBJECT (pix));

	return self;
}


static void
elpea_thumbnail_init (ElpeaThumbnail *self)
{
	self->priv = ELPEA_THUMBNAIL_GET_PRIVATE (self);
	ElpeaThumbnailPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


/* Thumbnail */


static GdkPixbuf *
elpea_thumbnail_scale (GdkPixbuf *pix)
{
	int fill = 1;
	const gint DW = 96;
	const gint DH = 64;
	gint w = gdk_pixbuf_get_width (pix);
	gint h = gdk_pixbuf_get_height (pix);

	gfloat rx = (float)w / (float)DW;
	gfloat ry = (float)h / (float)DH;


	GdkPixbuf *thumb;
	if (fill) {
		/* TODO: Use gdk_pixbuf_scale */
		gdouble r = MIN (rx, ry);

//		g_print ("rx=%f ry=%f, r=%f\n", rx, ry, r);

		gint dw, dh;
		if (rx > ry) {
//			g_print ("rx > ry\n");
			dw = w/r;
			dh = DH;
		} else {
//			g_print ("rx < ry\n");
			dw = DW;
			dh = h/r;
		}
		GdkPixbuf *t0 = gdk_pixbuf_scale_simple (pix, dw, dh, GDK_INTERP_BILINEAR);

//		g_print (" -> %d x %d trying to fit to %d x %d\n", dw, dh, DW, DH);

		gint dx = dw-DW;
		gint dy = dh-DH;

//		g_print ("dx = %d dy = %d\n", dx, dy);

		thumb = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, DW, DH);
		gdk_pixbuf_copy_area (t0, dx/2, dy/2, DW, DH, thumb, 0, 0);

		g_object_unref (t0);
	} else {
		gfloat r = MAX (rx, ry);

//		g_print ("rx=%f ry=%f, r=%f\n", rx, ry, r);

		gint dw = w/r;
		gint dh = h/r;
//		g_print ("dw=%d, dh=%d\n", dw, dh);

		thumb = gdk_pixbuf_scale_simple (pix, dw, dh, GDK_INTERP_BILINEAR);
	}
	return thumb;
}



const gchar *
elpea_thumbnail_get_name (ElpeaThumbnail *self)
{
	ElpeaThumbnailPrivate *priv = self->priv;
	return priv->name;
}


const gchar *
elpea_thumbnail_get_path (ElpeaThumbnail *self)
{
	ElpeaThumbnailPrivate *priv = self->priv;
	return priv->path;
}



gint
elpea_thumbnail_get_width (ElpeaThumbnail *self)
{
	ElpeaThumbnailPrivate *priv = self->priv;
	return priv->width;
}


gsize
elpea_thumbnail_get_height (ElpeaThumbnail *self)
{
	ElpeaThumbnailPrivate *priv = self->priv;
	return priv->height;
}


gsize
elpea_thumbnail_get_size (ElpeaThumbnail *self)
{
	ElpeaThumbnailPrivate *priv = self->priv;
	return priv->size;
}


GdkPixbuf *
elpea_thumbnail_get_pixbuf (ElpeaThumbnail *self)
{
	ElpeaThumbnailPrivate *priv = self->priv;
	return g_object_ref (priv->thumbnail);
}



/* GObject stuff */


static void
elpea_thumbnail_dispose (GObject *object)
{
	ElpeaThumbnail *self = (ElpeaThumbnail*) object;
	ElpeaThumbnailPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;

	g_free (priv->name);
	g_free (priv->path);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (elpea_thumbnail_parent_class)->dispose (object);
}


static void
elpea_thumbnail_finalize (GObject *object)
{
	G_OBJECT_CLASS (elpea_thumbnail_parent_class)->finalize (object);
}

	
static void
elpea_thumbnail_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ElpeaThumbnail* self = ELPEA_THUMBNAIL (object);
	ElpeaThumbnailPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_thumbnail_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ElpeaThumbnail* self = ELPEA_THUMBNAIL (object);
	ElpeaThumbnailPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_thumbnail_class_init (ElpeaThumbnailClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = elpea_thumbnail_get_property;
	gobject_class->set_property = elpea_thumbnail_set_property;
	gobject_class->dispose = elpea_thumbnail_dispose;
	gobject_class->finalize = elpea_thumbnail_finalize;

	g_type_class_add_private (klass, sizeof (ElpeaThumbnailPrivate));
}

