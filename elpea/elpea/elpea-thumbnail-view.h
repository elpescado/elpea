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

#ifndef __ELPEA_THUMBNAIL_VIEW_H__
#define __ELPEA_THUMBNAIL_VIEW_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define ELPEA_TYPE_THUMBNAIL_VIEW \
	(elpea_thumbnail_view_get_type ())
#define ELPEA_THUMBNAIL_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ELPEA_TYPE_THUMBNAIL_VIEW, ElpeaThumbnailView))
#define ELPEA_THUMBNAIL_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), ELPEA_TYPE_THUMBNAIL_VIEW, ElpeaThumbnailViewClass))
#define ELPEA_IS_THUMBNAIL_VIEW(obj) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), ELPEA_TYPE_THUMBNAIL_VIEW))
#define ELPEA_THUMBNAIL_VIEW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ELPEA_TYPE_THUMBNAIL_VIEW, ElpeaThumbnailViewClass))


typedef struct _ElpeaThumbnailView		ElpeaThumbnailView;
typedef struct _ElpeaThumbnailViewPrivate		ElpeaThumbnailViewPrivate;
typedef struct _ElpeaThumbnailViewClass		ElpeaThumbnailViewClass;

struct _ElpeaThumbnailView{
	GtkTreeView		parent;
	ElpeaThumbnailViewPrivate	*priv;
};

struct _ElpeaThumbnailViewClass
{
	GtkTreeViewClass		parent;

	/* Signals */
};



GType
elpea_thumbnail_view_get_type				(void);

GtkWidget*
elpea_thumbnail_view_new				(void);


G_END_DECLS

#endif
