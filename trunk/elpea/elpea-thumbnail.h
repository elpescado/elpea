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

#ifndef __ELPEA_THUMBNAIL_H__
#define __ELPEA_THUMBNAIL_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define ELPEA_TYPE_THUMBNAIL \
	(elpea_thumbnail_get_type ())
#define ELPEA_THUMBNAIL(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ELPEA_TYPE_THUMBNAIL, ElpeaThumbnail))
#define ELPEA_THUMBNAIL_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), ELPEA_TYPE_THUMBNAIL, ElpeaThumbnailClass))
#define ELPEA_IS_THUMBNAIL(obj) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), ELPEA_TYPE_THUMBNAIL))
#define ELPEA_THUMBNAIL_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ELPEA_TYPE_THUMBNAIL, ElpeaThumbnailClass))


typedef struct _ElpeaThumbnail		ElpeaThumbnail;
typedef struct _ElpeaThumbnailPrivate		ElpeaThumbnailPrivate;
typedef struct _ElpeaThumbnailClass		ElpeaThumbnailClass;

struct _ElpeaThumbnail{
	GObject		parent;
	ElpeaThumbnailPrivate	*priv;
};

struct _ElpeaThumbnailClass
{
	GObjectClass		parent;

	/* Signals */
};



GType
elpea_thumbnail_get_type				(void);

ElpeaThumbnail*
elpea_thumbnail_new				(const gchar *dirname, const gchar *file);


const gchar *
elpea_thumbnail_get_name (ElpeaThumbnail *self);

const gchar *
elpea_thumbnail_get_path (ElpeaThumbnail *self);

gint
elpea_thumbnail_get_width (ElpeaThumbnail *self);

gsize
elpea_thumbnail_get_height (ElpeaThumbnail *self);

gsize
elpea_thumbnail_get_size (ElpeaThumbnail *self);

GdkPixbuf *
elpea_thumbnail_get_pixbuf (ElpeaThumbnail *self);


void
elpea_thumbnail_load (ElpeaThumbnail *self);

G_END_DECLS

#endif
