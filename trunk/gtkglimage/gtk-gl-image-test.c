#include <stdlib.h>

#include <gtk/gtk.h>

#include "gtk-gl-image.h"


void
img_realized (GtkGlImage *img, const gchar *filename)
{
	g_print ("img_realized\n");
//	gtk_gl_image_set_from_file (GTK_GL_IMAGE (img), filename);
}

gboolean            scroll_cb                          (GtkWidget      *widget,
                                                        GdkEventScroll *event,
                                                        gpointer        user_data)
{
	GtkGlImage *img = GTK_GL_IMAGE (widget);
	g_print ("scroll (%d)\n", event->direction);
	if (event->direction == GDK_SCROLL_UP) {
		gtk_gl_image_zoom_in (img);
	} else if (event->direction == GDK_SCROLL_DOWN) {
		gtk_gl_image_zoom_out (img);
	}
}

int main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);
	gtk_gl_init (&argc, &argv);

	if (argv[1] == NULL) {
		g_printerr ("usage: %s image\n", argv[0]);
		return EXIT_FAILURE;
	}

	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "GtkGlImage demo");

	GtkWidget *img = gtk_gl_image_new ();
	gtk_widget_add_events (img, GDK_BUTTON_PRESS_MASK);
	gtk_widget_set_size_request (img, 128, 128);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 480);
	gtk_gl_image_set_from_file (GTK_GL_IMAGE (img), argv[1]);

	gtk_container_add (GTK_CONTAINER (window), img);

	g_signal_connect (G_OBJECT (window), "delete_event",
	                  G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT (img), "realize",
	                  G_CALLBACK (img_realized), argv[1]);
	g_signal_connect (G_OBJECT (img), "scroll-event",
	                  G_CALLBACK (scroll_cb), NULL);

	gtk_widget_show (img);
	gtk_widget_show (window);

	gtk_main ();

	return EXIT_SUCCESS;
}
