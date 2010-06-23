/*
 * elpea
 * -----
 *
 * Copyright (C) 2010 Przemysław Sitek
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __ELPEA_PREFERENCES_WINDOW_H__
#define __ELPEA_PREFERENCES_WINDOW_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "foo-prefs.h"


G_BEGIN_DECLS

#define ELPEA_TYPE_PREFERENCES_WINDOW \
	(elpea_preferences_window_get_type ())
#define ELPEA_PREFERENCES_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ELPEA_TYPE_PREFERENCES_WINDOW, ElpeaPreferencesWindow))
#define ELPEA_PREFERENCES_WINDOW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), ELPEA_TYPE_PREFERENCES_WINDOW, ElpeaPreferencesWindowClass))
#define ELPEA_IS_PREFERENCES_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ELPEA_TYPE_PREFERENCES_WINDOW))
#define ELPEA_IS_PREFERENCES_WINDOW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), ELPEA_TYPE_PREFERENCES_WINDOW))
#define ELPEA_PREFERENCES_WINDOW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ELPEA_TYPE_PREFERENCES_WINDOW, ElpeaPreferencesWindowClass))


typedef struct _ElpeaPreferencesWindow		ElpeaPreferencesWindow;
typedef struct _ElpeaPreferencesWindowPrivate		ElpeaPreferencesWindowPrivate;
typedef struct _ElpeaPreferencesWindowClass		ElpeaPreferencesWindowClass;

struct _ElpeaPreferencesWindow{
	GtkDialog		parent;
	ElpeaPreferencesWindowPrivate	*priv;
};

struct _ElpeaPreferencesWindowClass
{
	GtkDialogClass		parent;

	/* Signals */
};



GType
elpea_preferences_window_get_type				(void);

ElpeaPreferencesWindow*
elpea_preferences_window_new				(FooPrefs *prefs);


G_END_DECLS

#endif
