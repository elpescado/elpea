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

#ifndef __ELPEA_MAIN_WINDOW_H__
#define __ELPEA_MAIN_WINDOW_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define ELPEA_TYPE_MAIN_WINDOW \
	(elpea_main_window_get_type ())
#define ELPEA_MAIN_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ELPEA_TYPE_MAIN_WINDOW, ElpeaMainWindow))
#define ELPEA_MAIN_WINDOW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), ELPEA_TYPE_MAIN_WINDOW, ElpeaMainWindowClass))
#define ELPEA_IS_MAIN_WINDOW(obj) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), ELPEA_TYPE_MAIN_WINDOW))
#define ELPEA_MAIN_WINDOW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ELPEA_TYPE_MAIN_WINDOW, ElpeaMainWindowClass))


typedef struct _ElpeaMainWindow		ElpeaMainWindow;
typedef struct _ElpeaMainWindowPrivate		ElpeaMainWindowPrivate;
typedef struct _ElpeaMainWindowClass		ElpeaMainWindowClass;

struct _ElpeaMainWindow{
	GtkWindow		parent;
	ElpeaMainWindowPrivate	*priv;
};

struct _ElpeaMainWindowClass
{
	GtkWindowClass		parent;

	/* Signals */
};



GType
elpea_main_window_get_type				(void);

GtkWidget*
elpea_main_window_new				(void);


G_END_DECLS

#endif
