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

#include "elpea-directory.h"
#include "elpea-main-window.h"
#include "elpea-thumbnail-view.h"

#define N_(a) (a)
#define _(a) (a)

static void
zoom_adjustment_value_changed (GtkAdjustment *adjustment,
                               ElpeaMainWindow *self);


G_DEFINE_TYPE (ElpeaMainWindow, elpea_main_window, GTK_TYPE_WINDOW)

struct _ElpeaMainWindowPrivate
{
	/* Private members go here */
	GtkAdjustment *zoom_adjustment;

	/* Widgets */
	GtkActionGroup *action_group;
	GtkUIManager *ui;
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

void
dummy_callback (void)
{
	g_print (" *** dummy callback called ***\n");
}


static void
_action_about (GtkAction *action,
               gpointer   user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);

	GdkPixbuf *logo = gdk_pixbuf_new_from_file ("../data/elpea-logo.png", NULL);

	GtkWidget *win = gtk_about_dialog_new ();
	gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (self));
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (win), "elpea");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (win), "0.1");
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (win), "Copyright © 2009 Przemysław Sitek");
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (win), "OpenGL image viewer");
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (win), "GPL v3");
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (win), "http://code.google.com/p/elpea/");
	gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (win), _("translator-credits"));
//	gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (win), logo);

	/* TODO: Use standard icon, this fancy logo doesn't look good enough */
	GtkWidget *box = gtk_event_box_new ();
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (win)->vbox), box, FALSE, FALSE, 0);
	gtk_box_reorder_child (GTK_BOX (GTK_DIALOG (win)->vbox), box, 0);
	GdkColor black = {0, 0, 0, 0};
	gtk_widget_modify_bg (box, GTK_STATE_NORMAL, &black);

	GtkWidget *img = gtk_image_new ();
	gtk_image_set_from_pixbuf (GTK_IMAGE (img), logo);
	gtk_container_add (GTK_CONTAINER (box), img);

	gtk_widget_show (img);
	gtk_widget_show (box);


	g_object_unref (G_OBJECT (logo));

	gtk_dialog_run (GTK_DIALOG (win));
	gtk_widget_hide (win);
	gtk_widget_destroy (win);
}


/* UI actions */

static GtkAction *
_get_action (ElpeaMainWindow *self, const gchar *name)
{
	ElpeaMainWindowPrivate *priv = self->priv;
	return gtk_action_group_get_action (priv->action_group, name);
}


static const GtkActionEntry actions[] = {
	{"File", NULL, N_("File")},
		{"Open", GTK_STOCK_OPEN, NULL, "<Ctrl>o", N_("Open image"), G_CALLBACK (dummy_callback)},
		{"Quit", GTK_STOCK_QUIT, NULL, "<Ctrl>q", N_("Quit"), G_CALLBACK (gtk_main_quit)},
	{"Edit", NULL, N_("Edit")},
		{"Preferences", GTK_STOCK_PREFERENCES, NULL, "<Ctrl><Alt>p", N_("Preferences"), G_CALLBACK (dummy_callback)},
	{"View", NULL, N_("View")},
		{"ZoomIn",     GTK_STOCK_ZOOM_IN, NULL, "plus", N_("Zoom image in"), G_CALLBACK (dummy_callback)},
		{"ZoomOut",    GTK_STOCK_ZOOM_OUT, NULL, "minus", N_("Zoom image out"), G_CALLBACK (dummy_callback)},
		{"Zoom1",      GTK_STOCK_ZOOM_100, NULL, "1", N_("Normal zoom"), G_CALLBACK (dummy_callback)},
		{"ZoomFit",    GTK_STOCK_ZOOM_FIT, NULL, "f", N_("Fit to window"), G_CALLBACK (dummy_callback)},
		{"RotateLeft", "object-rotate-left", "Rotate Left", "l", N_("Rotate image counter-clockwise"), G_CALLBACK (dummy_callback)},
		{"RotateRight","object-rotate-right", "Rotate Right", "r", N_("Rotate image clockwise"), G_CALLBACK (dummy_callback)},
	{"Go", NULL, N_("Go")},
		{"Prev",    GTK_STOCK_GO_BACK, NULL, "<Alt>Left", N_("Previous image"), G_CALLBACK (dummy_callback)},
		{"Next",    GTK_STOCK_GO_FORWARD, NULL, "<Alt>Right", N_("Next image"), G_CALLBACK (dummy_callback)},
	{"Help", NULL, N_("Help")},
		{"About",   GTK_STOCK_ABOUT, NULL, NULL, N_("About elpea"), G_CALLBACK (_action_about)}

};

static const guint n_actions = G_N_ELEMENTS (actions);


static const GtkToggleActionEntry toggle_actions [] = {
	{"ShowMenubar", NULL, N_("Show Menubar"), "m", N_("Whether to show menubar"), G_CALLBACK (dummy_callback), TRUE},
	{"ShowToolbar", NULL, N_("Show Toolbar"), "", N_("Whether to show toolbar"), G_CALLBACK (dummy_callback), TRUE},
	{"ShowSidebar", NULL, N_("Show Sidebar"), "", N_("Whether to show sidebar"), G_CALLBACK (dummy_callback), TRUE},
	{"ShowStatusbar", NULL, N_("Show Statusbar"), "", N_("Whether to show statusbar"), G_CALLBACK (dummy_callback), TRUE},
};

static const guint n_toggle_actions = G_N_ELEMENTS (toggle_actions);



static const gchar* ui_markup =
"<ui>"
	"<menubar>"
		"<menu action='File'>"
			"<menuitem action='Open' />"
			"<separator/>"
			"<menuitem action='Quit' />"
		"</menu>"
		"<menu action='Edit'>"
			"<menuitem action='Preferences' />"
		"</menu>"
		"<menu action='View'>"
			"<menuitem action='ShowMenubar' />"
			"<menuitem action='ShowToolbar' />"
			"<menuitem action='ShowSidebar' />"
			"<menuitem action='ShowStatusbar' />"
			"<separator/>"
			"<menuitem action='ZoomIn' />"
			"<menuitem action='ZoomOut' />"
			"<menuitem action='Zoom1' />"
			"<menuitem action='ZoomFit' />"
			"<separator/>"
			"<menuitem action='RotateLeft' />"
			"<menuitem action='RotateRight' />"
		"</menu>"
		"<menu action='Go'>"
			"<menuitem action='Prev' />"
			"<menuitem action='Next' />"
		"</menu>"
		"<menu action='Help'>"
			"<menuitem action='About' />"
		"</menu>"
	"</menubar>"
	"<toolbar>"
		"<toolitem action='Open' />"
		"<separator/>"
		"<toolitem action='Prev' />"
		"<toolitem action='Next' />"
		"<separator/>"
		"<toolitem action='ZoomIn' />"
		"<toolitem action='ZoomOut' />"
		"<toolitem action='Zoom1' />"
		"<toolitem action='ZoomFit' />"
		"<separator/>"
		"<toolitem action='RotateLeft' />"
		"<toolitem action='RotateRight' />"
	"</toolbar>"
"</ui>";


static void
elpea_main_window_init_actions (ElpeaMainWindow *self)
{
	ElpeaMainWindowPrivate *priv = self->priv;
	GError *error = NULL;

	priv->action_group = gtk_action_group_new ("elpea");
	//gtk_action_group_set_translation_domain (browser->action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (priv->action_group, actions, n_actions, self);
	gtk_action_group_add_toggle_actions (priv->action_group, toggle_actions, n_toggle_actions, self);

	/* Setup some actions */
#if GTK_CHECK_VERSION (2, 16, 0)
	gtk_action_set_is_important (_get_action (self, "Prev"), TRUE);
	gtk_action_set_is_important (_get_action (self, "Next"), TRUE);
#endif

	priv->ui = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (priv->ui, priv->action_group, 0);
	gtk_window_add_accel_group (GTK_WINDOW (self), gtk_ui_manager_get_accel_group (priv->ui));

	if (! gtk_ui_manager_add_ui_from_string (priv->ui, ui_markup, -1, &error)) {
		g_printerr (" *** ERROR??? %s\n", error->message);
		g_error_free (error);
	}

}


static void
elpea_main_window_init_gui (ElpeaMainWindow *self)
{
	ElpeaMainWindowPrivate *priv = self->priv;

	elpea_main_window_init_actions (self);
	gtk_window_set_title (GTK_WINDOW (self), "elpea Photo Album");
	gtk_window_set_default_size (GTK_WINDOW (self), 640, 480);


	/* Main vbox */
	GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (self), vbox);
	gtk_widget_show (vbox);

	/* Main menu placeholder */
//	GtkWidget *menubar = gtk_menu_new ();
	GtkWidget *menubar = gtk_ui_manager_get_widget (priv->ui, "/menubar");
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);	// FIXME: Warning here
	gtk_widget_show (menubar);

	/* Toolbar placeholder */
//	GtkWidget *toolbar = gtk_toolbar_new ();
	GtkWidget *toolbar = gtk_ui_manager_get_widget (priv->ui, "/toolbar");
	g_print ("toolbar=%p\n", toolbar);
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
	gtk_gl_image_set_from_file (GTK_GL_IMAGE (img), "../data/elpea-logo.png");
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

//	priv->thumbnail_model = create_tree_model ();
	ElpeaDirectory *dir = elpea_directory_new ();
	g_print ("dir=%p (%d)\n", dir, ELPEA_IS_DIRECTORY (dir));
	elpea_directory_load (dir, "..");
	priv->thumbnail_model = dir;

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

