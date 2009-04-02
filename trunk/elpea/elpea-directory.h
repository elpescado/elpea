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

#ifndef __ELPEA_DIRECTORY_H__
#define __ELPEA_DIRECTORY_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define ELPEA_TYPE_DIRECTORY \
	(elpea_directory_get_type ())
#define ELPEA_DIRECTORY(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ELPEA_TYPE_DIRECTORY, ElpeaDirectory))
#define ELPEA_DIRECTORY_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), ELPEA_TYPE_DIRECTORY, ElpeaDirectoryClass))
#define ELPEA_IS_DIRECTORY(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ELPEA_TYPE_DIRECTORY))
#define ELPEA_DIRECTORY_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ELPEA_TYPE_DIRECTORY, ElpeaDirectoryClass))


typedef struct _ElpeaDirectory		ElpeaDirectory;
typedef struct _ElpeaDirectoryPrivate		ElpeaDirectoryPrivate;
typedef struct _ElpeaDirectoryClass		ElpeaDirectoryClass;

struct _ElpeaDirectory{
	GtkListStore		parent;
	ElpeaDirectoryPrivate	*priv;
};

struct _ElpeaDirectoryClass
{
	GtkListStoreClass		parent;

	/* Signals */
};



GType
elpea_directory_get_type				(void);

ElpeaDirectory*
elpea_directory_new				(void);


void
elpea_directory_load (ElpeaDirectory *self,
                      const gchar *dirname);

G_END_DECLS

#endif
