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

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "elpea-preferences-window.h"

#include "baz-prefs.h"


G_DEFINE_TYPE (ElpeaPreferencesWindow, elpea_preferences_window, GTK_TYPE_DIALOG)

struct _ElpeaPreferencesWindowPrivate
{
	/* Private members go here */
	FooPrefs *prefs;

	gboolean disposed;
};

static void
elpea_preferences_window_init_gui (ElpeaPreferencesWindow *self);


#define ELPEA_PREFERENCES_WINDOW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	ELPEA_TYPE_PREFERENCES_WINDOW, ElpeaPreferencesWindowPrivate))


ElpeaPreferencesWindow*
elpea_preferences_window_new (FooPrefs *prefs)
{
	ElpeaPreferencesWindow *self = g_object_new (ELPEA_TYPE_PREFERENCES_WINDOW, NULL);
	ElpeaPreferencesWindowPrivate *priv = self->priv;
	priv->prefs = prefs;
	elpea_preferences_window_init_gui (self);
	return self;
}


static void
elpea_preferences_window_init (ElpeaPreferencesWindow *self)
{
	self->priv = ELPEA_PREFERENCES_WINDOW_GET_PRIVATE (self);
	ElpeaPreferencesWindowPrivate *priv = self->priv;

	priv->disposed = FALSE;

}


static void
elpea_preferences_window_init_gui (ElpeaPreferencesWindow *self)
{
	ElpeaPreferencesWindowPrivate *priv = self->priv;
	GtkWidget *close = gtk_dialog_add_button (GTK_DIALOG (self),
	                                          GTK_STOCK_CLOSE,
	                                          GTK_RESPONSE_CLOSE);
	g_signal_connect_swapped (close, "clicked", G_CALLBACK (gtk_widget_hide), self);

	gtk_dialog_set_has_separator (GTK_DIALOG (self), FALSE);
	gtk_window_set_title (GTK_WINDOW (self), _("Preferences"));
	gtk_window_set_resizable (GTK_WINDOW (self), FALSE);

	GtkWidget *notebook = gtk_notebook_new ();
	gtk_container_set_border_width (GTK_CONTAINER (notebook), 12);
	gtk_notebook_set_show_tabs (GTK_CONTAINER (notebook), FALSE);
	gtk_notebook_set_show_border (GTK_CONTAINER (notebook), FALSE);
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (GTK_DIALOG (self))), notebook);
	gtk_widget_show (notebook);

	GtkWidget *t1 = gtk_table_new (2, 2, FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (t1), 12);
	gtk_table_set_row_spacings (GTK_TABLE (t1), 6);
	gtk_table_set_col_spacings (GTK_TABLE (t1), 6);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), t1, gtk_label_new (_("General")));
	gtk_widget_show (t1);

	GtkWidget *label1 = gtk_label_new (_("<b>Scaling</b>"));
	gtk_misc_set_alignment (GTK_MISC (label1), 0.0f, 0.5f);
	gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
	gtk_table_attach (GTK_TABLE (t1), label1, 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_widget_show (label1);

	GtkWidget *zoom_stretch = gtk_check_button_new_with_label (_("Stretch images when fitting to window"));
	gtk_table_attach (GTK_TABLE (t1), zoom_stretch, 0, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_widget_show (zoom_stretch);

	baz_prefs_bind_widget (priv->prefs, zoom_stretch, "zoom_fit_stretch");

}


static void
elpea_preferences_window_dispose (GObject *object)
{
	ElpeaPreferencesWindow *self = (ElpeaPreferencesWindow*) object;
	ElpeaPreferencesWindowPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (elpea_preferences_window_parent_class)->dispose (object);
}


static void
elpea_preferences_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (elpea_preferences_window_parent_class)->finalize (object);
}

	
static void
elpea_preferences_window_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ElpeaPreferencesWindow* self = ELPEA_PREFERENCES_WINDOW (object);
	ElpeaPreferencesWindowPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_preferences_window_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ElpeaPreferencesWindow* self = ELPEA_PREFERENCES_WINDOW (object);
	ElpeaPreferencesWindowPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_preferences_window_class_init (ElpeaPreferencesWindowClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = elpea_preferences_window_get_property;
	gobject_class->set_property = elpea_preferences_window_set_property;
	gobject_class->dispose = elpea_preferences_window_dispose;
	gobject_class->finalize = elpea_preferences_window_finalize;

	g_type_class_add_private (klass, sizeof (ElpeaPreferencesWindowPrivate));
}

