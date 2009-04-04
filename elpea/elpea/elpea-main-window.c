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

#include <math.h>

#include <gtk/gtk.h>

#include <gtk-gl-image.h>

#include "elpea-main-window.h"
#include "elpea-thumbnail-view.h"

static void
zoom_adjustment_value_changed (GtkAdjustment *adjustment,
                               ElpeaMainWindow *self);


G_DEFINE_TYPE (ElpeaMainWindow, elpea_main_window, GTK_TYPE_WINDOW)

struct _ElpeaMainWindowPrivate
{
	/* Private members go here */
	GtkAdjustment *zoom_adjustment;

	/* Widgets */
	GtkGlImage *image;

	GtkTreeModel *thumbnail_model;

	gboolean disposed;
};


#define ELPEA_MAIN_WINDOW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	ELPEA_TYPE_MAIN_WINDOW, ElpeaMainWindowPrivate))


GtkWidget*
elpea_main_window_new (void)
{
	ElpeaMainWindow *self = g_object_new (ELPEA_TYPE_MAIN_WINDOW, NULL);
	return GTK_WIDGET (self);
}


static void
elpea_main_window_init_gui (ElpeaMainWindow *self)
{
	ElpeaMainWindowPrivate *priv = self->priv;

	gtk_window_set_title (GTK_WINDOW (self), "elpea Photo Album");
	gtk_window_set_default_size (GTK_WINDOW (self), 640, 480);


	/* Main vbox */
	GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (self), vbox);
	gtk_widget_show (vbox);

	/* Main menu placeholder */
	GtkWidget *menubar = gtk_menu_new ();
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
	gtk_widget_show (menubar);

	/* Toolbar placeholder */
	GtkWidget *toolbar = gtk_toolbar_new ();
	gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
	gtk_widget_show (toolbar);

	/*
	 * Image area
	 */

	/* Paned */
	GtkWidget *paned = gtk_hpaned_new ();
	gtk_box_pack_start (GTK_BOX (vbox), paned, TRUE, TRUE, 0);
	gtk_widget_show (paned);

	/* Thumbnail list */
	GtkWidget *sw1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw1), GTK_SHADOW_IN);
	gtk_paned_add1 (GTK_PANED (paned), sw1);
	gtk_widget_show (sw1);

	GtkWidget *thumb_view = elpea_thumbnail_view_new ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (thumb_view), priv->thumbnail_model);
	gtk_container_add (GTK_CONTAINER (sw1), thumb_view);
	gtk_widget_show (thumb_view);

	/* Image preview */
	GtkWidget *img = gtk_gl_image_new ();
	gtk_gl_image_set_from_file (GTK_GL_IMAGE (img), "../gtkglimage/lighthouse.jpg");
	gtk_widget_add_events (img, GDK_BUTTON_PRESS_MASK);
	gtk_paned_add2 (GTK_PANED (paned), img);
	gtk_widget_show (img);
	priv->image = GTK_GL_IMAGE (img);

	gtk_paned_set_position (GTK_PANED (paned), 200);
	/*
	 * Status bar
	 */

	/* Status box */
	GtkWidget *status_box = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (vbox), status_box, FALSE, FALSE, 0);
	gtk_widget_show (status_box);

	/* Status part */
	GtkWidget *main_status = gtk_statusbar_new ();
	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (main_status), FALSE);
	gtk_box_pack_start (GTK_BOX (status_box), main_status, TRUE, TRUE, 0);
	gtk_widget_show (main_status);

	/* Zoom bar */
	GtkWidget *zoom_out_img = gtk_image_new_from_stock (GTK_STOCK_ZOOM_OUT, GTK_ICON_SIZE_MENU);
	gtk_box_pack_start (GTK_BOX (status_box), zoom_out_img, FALSE, FALSE, 0);
	gtk_widget_show (zoom_out_img);

	GtkWidget *slider = gtk_hscale_new (priv->zoom_adjustment);
	gtk_scale_set_draw_value (GTK_SCALE (slider), FALSE);
	gtk_widget_set_size_request (slider, 160, 12);
	gtk_box_pack_start (GTK_BOX (status_box), slider, FALSE, FALSE, 0);
	gtk_widget_show (slider);

	GtkWidget *zoom_in_img = gtk_image_new_from_stock (GTK_STOCK_ZOOM_IN, GTK_ICON_SIZE_MENU);
	gtk_box_pack_start (GTK_BOX (status_box), zoom_in_img, FALSE, FALSE, 0);
	gtk_widget_show (zoom_in_img);

	GtkWidget *zoom_status = gtk_statusbar_new ();
	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (zoom_status), TRUE);
	gtk_box_pack_start (GTK_BOX (status_box), zoom_status, FALSE, FALSE, 0);
	gtk_widget_show (zoom_status);


}


static void
elpea_main_window_init (ElpeaMainWindow *self)
{
	self->priv = ELPEA_MAIN_WINDOW_GET_PRIVATE (self);
	ElpeaMainWindowPrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->zoom_adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, -3.0, 3.0, 0.25, 1.0, 0.0));
	g_signal_connect (G_OBJECT (priv->zoom_adjustment), "value-changed",
	                  G_CALLBACK (zoom_adjustment_value_changed), self);

	priv->thumbnail_model = create_tree_model ();

	elpea_main_window_init_gui (self);
}


/*
 * Callbacks
 */

static void
zoom_adjustment_value_changed (GtkAdjustment *adjustment,
                               ElpeaMainWindow *self)
{
	ElpeaMainWindowPrivate *priv = self->priv;
	double val = gtk_adjustment_get_value (adjustment);
	double zoom = pow (2.0, val);
	g_print ("value = %lf zoom = %lf\n", val, zoom);
	g_print ("zoom_adjustment_value_changed: image=%p\n", priv->image);
	gtk_gl_image_set_zoom (priv->image, zoom);
}


static void
elpea_main_window_dispose (GObject *object)
{
	ElpeaMainWindow *self = (ElpeaMainWindow*) object;
	ElpeaMainWindowPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (elpea_main_window_parent_class)->dispose (object);
}


static void
elpea_main_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (elpea_main_window_parent_class)->finalize (object);
}

	
static void
elpea_main_window_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ElpeaMainWindow* self = ELPEA_MAIN_WINDOW (object);
	ElpeaMainWindowPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_main_window_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ElpeaMainWindow* self = ELPEA_MAIN_WINDOW (object);
	ElpeaMainWindowPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
elpea_main_window_class_init (ElpeaMainWindowClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = elpea_main_window_get_property;
	gobject_class->set_property = elpea_main_window_set_property;
	gobject_class->dispose = elpea_main_window_dispose;
	gobject_class->finalize = elpea_main_window_finalize;

	g_type_class_add_private (klass, sizeof (ElpeaMainWindowPrivate));
}
