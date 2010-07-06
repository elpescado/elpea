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
#include <string.h>

#include <gtk/gtk.h>

#include <gtk-gl-image.h>

#include "elpea-directory.h"
#include "elpea-main-window.h"
#include "elpea-thumbnail-view.h"
#include "elpea-thumbnail.h"
#include "elpea-preferences-window.h"

#include "ooze-cache.h"
#include "foo-prefs.h"
#include "baz-prefs.h"


#define N_(a) (a)
#define _(a) (a)

#define STCXID 0x17

static void
zoom_adjustment_value_changed (GtkAdjustment *adjustment,
                               ElpeaMainWindow *self);

static void
thumb_view_selection_changed_cb (GtkTreeSelection *selection, ElpeaMainWindow *data);


static void
elpea_main_window_load_file (ElpeaMainWindow *self, const gchar *path);

static void
elpea_main_window_load_dir (ElpeaMainWindow *self, const gchar *path);

static void
elpea_main_window_preferences_notify (FooPrefs *prefs, const gchar *key, gpointer data);

static void
elpea_main_window_update_open_with_menu (ElpeaMainWindow *self,
                                         const gchar     *file_name);



G_DEFINE_TYPE (ElpeaMainWindow, elpea_main_window, GTK_TYPE_WINDOW)

struct _ElpeaMainWindowPrivate
{
	/* Private members go here */
	GtkAdjustment *zoom_adjustment;
	gulong         zoom_changed_handler_id;

	/* Widgets */
	GtkWidget       *menubar;
	GtkWidget       *toolbar;
	GtkWidget       *statusbar;
	GtkWidget       *sidebar;
	GtkActionGroup  *action_group;
	GtkUIManager    *ui;
	GtkGlImage      *image;
	GtkWidget       *zoom_statusbar;
	GtkWidget       *thumb_view;
	GtkWidget       *open_with_menu_item;

	GtkTreeModel    *thumbnail_model;

	OozeCache       *pixbuf_cache;

	GtkWidget       *prefs_window;

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
dummy_callback (GtkAction *action,
                gpointer   user_data)
{
	const gchar *name = gtk_action_get_name (action);
	g_print (" *** dummy callback for action '%s' called ***\n", name);
}


static void
_zoom_changed (GObject    *gobject,
               GParamSpec *pspec,
	       gpointer    user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);
	ElpeaMainWindowPrivate *priv = self->priv;

	double zoom = gtk_gl_image_get_zoom (GTK_GL_IMAGE (priv->image));

	gchar msg[10];
	snprintf (msg, 10, "%d%%", (int)(100*zoom));

	gtk_statusbar_pop (GTK_STATUSBAR (priv->zoom_statusbar), STCXID);
	gtk_statusbar_push (GTK_STATUSBAR (priv->zoom_statusbar), STCXID, msg);

	double zoom_linear = log (zoom) / log (2.0);
	//g_signal_stop_emission_by_name (priv->zoom_adjustment, "value-changed");
	g_signal_handler_block (priv->zoom_adjustment, priv->zoom_changed_handler_id);
	gtk_adjustment_set_value (priv->zoom_adjustment, zoom_linear);
	g_signal_handler_unblock (priv->zoom_adjustment, priv->zoom_changed_handler_id);

	static gint i = 0;
	g_print ("Zoom changed: %d (%lf, %lf)\n", i++, zoom, zoom_linear);
}


/* UI Action callbacks */

static void
_action_open (GtkAction *action,
              gpointer   user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);

	GtkWidget *win = gtk_file_chooser_dialog_new ("Open file",
	                                              GTK_WINDOW (self),
						      GTK_FILE_CHOOSER_ACTION_OPEN,
						      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						      GTK_STOCK_OPEN, GTK_RESPONSE_OK,
						      NULL);
	gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);
	int code = gtk_dialog_run (GTK_DIALOG (win));
	gtk_widget_hide (win);

	if (code == GTK_RESPONSE_OK) {
		gchar *file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (win));
		gchar *dir  = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (win));

		elpea_main_window_load_file (self, file);
		elpea_main_window_load_dir (self, dir);

		g_free (file);
		g_free (dir);
	}

	gtk_widget_destroy (win);
}


static void
_action_preferences (GtkAction *action,
                     gpointer   user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);
	ElpeaMainWindowPrivate *priv = self->priv;

	if (priv->prefs_window == NULL) {
		extern FooPrefs *prefs;
		priv->prefs_window = elpea_preferences_window_new (prefs);
		gtk_window_set_transient_for (GTK_WINDOW (priv->prefs_window), GTK_WINDOW (self));
		g_signal_connect (priv->prefs_window, "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	}

	gtk_widget_show (priv->prefs_window);
}


static void
_action_about (GtkAction *action,
               gpointer   user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);

	GdkPixbuf *logo = gdk_pixbuf_new_from_file ("../data/elpea-icon.png", NULL);

	GtkWidget *win = gtk_about_dialog_new ();
	gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (self));
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (win), "elpea");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (win), "0.1");
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (win), "Copyright © 2009 Przemysław Sitek");
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (win), "OpenGL image viewer");
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (win), "GPL v3");
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (win), "http://code.google.com/p/elpea/");
	gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (win), _("translator-credits"));
	gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (win), logo);

	/* TODO: Use standard icon, this fancy logo doesn't look good enough */
	/*
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
		*/

	g_object_unref (G_OBJECT (logo));

	gtk_dialog_run (GTK_DIALOG (win));
	gtk_widget_hide (win);
	gtk_widget_destroy (win);
}

static void
_action_zoom  (GtkAction *action,
               gpointer   user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);
	ElpeaMainWindowPrivate *priv = self->priv;
	GtkGlImage *img = GTK_GL_IMAGE (priv->image);
	extern FooPrefs *prefs;

	const gchar *name = gtk_action_get_name (action);
	if (strcmp (name, "ZoomIn") == 0)
		gtk_gl_image_zoom_in (img);
	else if (strcmp (name, "ZoomOut") == 0)
		gtk_gl_image_zoom_out (img);
	else if (strcmp (name, "Zoom1") == 0)
		gtk_gl_image_set_zoom (img, 1.0);
	else if (strcmp (name, "ZoomFit") == 0)
		gtk_gl_image_zoom_fit (img, foo_prefs_get_bool (prefs, "zoom_fit_stretch", FALSE));
	else 
		g_print ("Invalid action: '%s'\n", name);
}

static void
_action_rotate  (GtkAction *action,
               gpointer   user_data)
{
	ElpeaMainWindow *self = ELPEA_MAIN_WINDOW (user_data);
	ElpeaMainWindowPrivate *priv = self->priv;
	GtkGlImage *img = GTK_GL_IMAGE (priv->image);

	const gchar *name = gtk_action_get_name (action);

	if (strcmp (name, "RotateLeft") == 0)
		gtk_gl_image_rotate_left (img);
	else if (strcmp (name, "RotateRight") == 0)
		gtk_gl_image_rotate_right (img);
	else 
		g_print ("Invalid action: '%s'\n", name);
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
		{"Open", GTK_STOCK_OPEN, NULL, "<Ctrl>o", N_("Open image"), G_CALLBACK (_action_open)},
		{"OpenWith", NULL, N_("Open With"), NULL, N_("Open using external application"), NULL},
		{"Quit", GTK_STOCK_QUIT, NULL, "<Ctrl>q", N_("Quit"), G_CALLBACK (gtk_main_quit)},
	{"Edit", NULL, N_("Edit")},
		{"Preferences", GTK_STOCK_PREFERENCES, NULL, "<Ctrl><Alt>p", N_("Preferences"), G_CALLBACK (_action_preferences)},
	{"View", NULL, N_("View")},
		{"ZoomIn",     GTK_STOCK_ZOOM_IN, NULL, "plus", N_("Zoom image in"), G_CALLBACK (_action_zoom)},
		{"ZoomOut",    GTK_STOCK_ZOOM_OUT, NULL, "minus", N_("Zoom image out"), G_CALLBACK (_action_zoom)},
		{"Zoom1",      GTK_STOCK_ZOOM_100, NULL, "1", N_("Normal zoom"), G_CALLBACK (_action_zoom)},
		{"ZoomFit",    GTK_STOCK_ZOOM_FIT, NULL, "f", N_("Fit to window"), G_CALLBACK (_action_zoom)},
		{"RotateLeft", "object-rotate-left", "Rotate Left", "l", N_("Rotate image counter-clockwise"), G_CALLBACK (_action_rotate)},
		{"RotateRight","object-rotate-right", "Rotate Right", "r", N_("Rotate image clockwise"), G_CALLBACK (_action_rotate)},
	{"Go", NULL, N_("Go")},
		{"Prev",    GTK_STOCK_GO_BACK, NULL, "<Alt>Left", N_("Previous image"), G_CALLBACK (dummy_callback)},
		{"Next",    GTK_STOCK_GO_FORWARD, NULL, "<Alt>Right", N_("Next image"), G_CALLBACK (dummy_callback)},
	{"Help", NULL, N_("Help")},
		{"About",   GTK_STOCK_ABOUT, NULL, NULL, N_("About elpea"), G_CALLBACK (_action_about)}

};

static const guint n_actions = G_N_ELEMENTS (actions);


static const GtkToggleActionEntry toggle_actions [] = {
	{"ShowMenubar", NULL, N_("Show Menubar"), "m", N_("Whether to show menubar"), NULL, TRUE},
	{"ShowToolbar", NULL, N_("Show Toolbar"), "", N_("Whether to show toolbar"), NULL, TRUE},
	{"ShowSidebar", NULL, N_("Show Sidebar"), "", N_("Whether to show sidebar"), NULL, TRUE},
	{"ShowStatusbar", NULL, N_("Show Statusbar"), "", N_("Whether to show statusbar"), NULL, TRUE},
};

static const guint n_toggle_actions = G_N_ELEMENTS (toggle_actions);



static const gchar* ui_markup =
"<ui>"
	"<menubar>"
		"<menu action='File'>"
			"<menuitem action='Open' />"
			"<menu action='OpenWith'>"
				"<menuitem action='Quit'/>"
			"</menu>"
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
	extern FooPrefs *prefs;

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
	baz_prefs_bind_action (prefs, _get_action(self, "ShowSidebar"), "show_sidebar");
	baz_prefs_bind_action (prefs, _get_action(self, "ShowMenubar"), "show_menubar");
	baz_prefs_bind_action (prefs, _get_action(self, "ShowToolbar"), "show_toolbar");
	baz_prefs_bind_action (prefs, _get_action(self, "ShowStatusbar"), "show_statusbar");

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
	GtkWidget *menubar = gtk_ui_manager_get_widget (priv->ui, "/menubar");
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);	// FIXME: Warning here
	gtk_widget_show (menubar);
	priv->menubar = menubar;

	/* Toolbar placeholder */
	GtkWidget *toolbar = gtk_ui_manager_get_widget (priv->ui, "/toolbar");
	gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
	gtk_widget_show (toolbar);
	priv->toolbar = toolbar;

	GtkWidget *open_with_menu_item = gtk_ui_manager_get_widget (priv->ui, "/menubar/File/OpenWith");
	priv->open_with_menu_item = open_with_menu_item;

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
	priv->sidebar = sw1;

	GtkWidget *thumb_view = elpea_thumbnail_view_new ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (thumb_view), priv->thumbnail_model);
	gtk_container_add (GTK_CONTAINER (sw1), thumb_view);
	gtk_widget_show (thumb_view);
	priv->thumb_view = thumb_view;

	GtkTreeSelection *select;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (thumb_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
	                  G_CALLBACK (thumb_view_selection_changed_cb), self);


	/* Image preview */
	GtkWidget *sw2 = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw2), GTK_SHADOW_IN);
	gtk_paned_add2 (GTK_PANED (paned), sw2);
	gtk_widget_show (sw2);

	GtkWidget *img = gtk_gl_image_new ();
	gtk_gl_image_set_from_file (GTK_GL_IMAGE (img), "../data/elpea-logo.png");
	gtk_widget_add_events (img, GDK_BUTTON_PRESS_MASK);
	g_signal_connect (G_OBJECT (img), "notify::zoom",		// TODO: check if this makes UI laggy
	                  G_CALLBACK (_zoom_changed), self);
	gtk_container_add (GTK_CONTAINER (sw2), img);
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
	priv->statusbar = status_box;

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
	gtk_widget_set_size_request (zoom_status, 64, 12);
	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (zoom_status), TRUE);
	gtk_statusbar_push (GTK_STATUSBAR (zoom_status), STCXID, "");
	gtk_box_pack_start (GTK_BOX (status_box), zoom_status, FALSE, FALSE, 0);
	gtk_widget_show (zoom_status);
	priv->zoom_statusbar = zoom_status;


}

static void _row_changed                               (GtkTreeModel *tree_model,
                                                        GtkTreePath  *path,
                                                        GtkTreeIter  *iter,
                                                        gpointer      user_data)
{
	g_print (" *** Row %s changed\n", gtk_tree_path_to_string (path));
}


static void
elpea_main_window_init (ElpeaMainWindow *self)
{
	extern FooPrefs *prefs;

	self->priv = ELPEA_MAIN_WINDOW_GET_PRIVATE (self);
	ElpeaMainWindowPrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->pixbuf_cache = ooze_cache_new ();

	priv->zoom_adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, -3.0, 3.0, 0.25, 1.0, 0.0));
	priv->zoom_changed_handler_id =	g_signal_connect (
			          G_OBJECT (priv->zoom_adjustment), "value-changed",
	                  G_CALLBACK (zoom_adjustment_value_changed), self);

	ElpeaDirectory *dir = elpea_directory_new ();
	g_signal_connect (G_OBJECT (dir), "row-changed", G_CALLBACK (_row_changed), self);
	elpea_directory_load (dir, "/home/przemek/pliki/pix");
	priv->thumbnail_model = dir;

	foo_prefs_add_watch (prefs, "show_menubar", elpea_main_window_preferences_notify, self);
	foo_prefs_add_watch (prefs, "show_toolbar", elpea_main_window_preferences_notify, self);
	foo_prefs_add_watch (prefs, "show_statusbar", elpea_main_window_preferences_notify, self);
	foo_prefs_add_watch (prefs, "show_sidebar", elpea_main_window_preferences_notify, self);

	elpea_main_window_init_gui (self);
	_zoom_changed (NULL, NULL, self);

	/* Apply settings for the first time */
	elpea_main_window_preferences_notify (prefs, "show_menubar", self);
	elpea_main_window_preferences_notify (prefs, "show_toolbar", self);
	elpea_main_window_preferences_notify (prefs, "show_statusbar", self);
	elpea_main_window_preferences_notify (prefs, "show_sidebar", self);
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
	gtk_gl_image_set_zoom (priv->image, zoom);
}


static void
thumb_view_selection_changed_cb (GtkTreeSelection *selection, ElpeaMainWindow *self)
{
	ElpeaMainWindowPrivate *priv = self->priv;

	GtkTreeIter iter;
	GtkTreeModel *model;
	ElpeaThumbnail *thumb;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 0, &thumb, -1);
		const char *path = elpea_thumbnail_get_path (thumb);
		elpea_main_window_load_file (self, path);
		g_object_unref (thumb);
	}

}


static void
elpea_main_window_preferences_notify (FooPrefs    *prefs,
                                      const gchar *key,
                                      gpointer     data)
{
	ElpeaMainWindowPrivate *priv = ELPEA_MAIN_WINDOW(data)->priv;

	GtkWidget *widget = NULL;
	
	if (strcmp (key, "show_menubar") == 0) {
		widget = priv->menubar;
	} else if (strcmp (key, "show_toolbar") == 0) {
		widget = priv->toolbar;
	} else if (strcmp (key, "show_statusbar") == 0) {
		widget = priv->statusbar;
	} else if (strcmp (key, "show_sidebar") == 0) {
		widget = priv->sidebar;
	}

	if (widget) {
		gtk_widget_set_visible (widget, foo_prefs_get_bool (prefs, key, TRUE));
	}
	
}



/* 
 * Window functions
 */

static void
elpea_main_window_load_file (ElpeaMainWindow *self, const gchar *path)
{
	ElpeaMainWindowPrivate *priv = self->priv;

	GdkPixbuf *pix = load_pixbuf (priv->pixbuf_cache, path);
	gtk_gl_image_set_from_pixbuf (GTK_GL_IMAGE (priv->image), pix);
	g_object_unref (pix);
	//gtk_gl_image_set_from_file (GTK_GL_IMAGE (priv->image), path);

	elpea_main_window_update_open_with_menu (self, path);
}


static void
elpea_main_window_load_dir (ElpeaMainWindow *self,
                            const gchar     *path)
{
	ElpeaMainWindowPrivate *priv = self->priv;

	ElpeaDirectory *dir = elpea_directory_new ();
	elpea_directory_load (dir, path);
	priv->thumbnail_model = GTK_TREE_MODEL (dir);
	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->thumb_view), priv->thumbnail_model);
}


/*
 * Open with...
 */

static void
elpea_main_window_open_with_activated (GtkMenuItem *menuitem,
                                       gpointer     user_data)
{
	GAppInfo *app_info     = g_object_get_data (G_OBJECT (menuitem), "AppInfo");
	GFile *file = g_object_get_data (G_OBJECT (menuitem), "File");

	g_return_if_fail (app_info != NULL);
	g_return_if_fail (file != NULL);

	GList list = {NULL};
	list.data = file;

	g_app_info_launch (app_info, &list, NULL, NULL);
}


static void
elpea_main_window_update_open_with_menu (ElpeaMainWindow *self,
                                         const gchar     *file_name)
{
	ElpeaMainWindowPrivate *priv = self->priv;

	g_print ("elpea_main_window_update_open_with_menu ('%s')\n", file_name);
	GFile *file = g_file_new_for_path (file_name);
	GFileInfo *info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
	                                     G_FILE_QUERY_INFO_NONE, NULL, NULL);
	const gchar* content_type = g_file_info_get_content_type (info);
	g_print ("Content type = '%s'\n", content_type);

	GtkWidget *menu = gtk_menu_new ();

	GList *app_infos = g_app_info_get_all_for_type (content_type);
	for (GList *node = app_infos; node; node = node->next) {
		GAppInfo *app_info = (GAppInfo *)node->data;

#if GLIB_CHECK_VERSION (2, 24, 0)
		const gchar *app_name = g_app_info_get_display_name (app_info);
#else
		const gchar *app_name = g_app_info_get_name (app_info);
#endif

		GtkWidget *menu_item = gtk_image_menu_item_new_with_label (app_name);
		g_object_set_data_full (G_OBJECT (menu_item), "AppInfo", app_info, g_object_unref);
		g_object_set_data_full (G_OBJECT (menu_item), "File", g_object_ref (file), g_object_unref);
		g_signal_connect (menu_item, "activate", G_CALLBACK (elpea_main_window_open_with_activated), self);
		GtkWidget *icon = gtk_image_new_from_gicon (g_app_info_get_icon (app_info), GTK_ICON_SIZE_MENU);
		gtk_widget_show (icon);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item), icon);
#if GTK_CHECK_VERSION (2, 16, 0)
		gtk_image_menu_item_set_always_show_image (GTK_IMAGE_MENU_ITEM (menu_item), TRUE);
#endif
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		gtk_widget_show (menu_item);
	}

	gtk_widget_show (menu);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->open_with_menu_item), GTK_MENU (menu));

	g_object_unref (info);
	g_object_unref (file);
}


/*
 * GObject stuff
 */

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

	g_object_unref (priv->pixbuf_cache);

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

