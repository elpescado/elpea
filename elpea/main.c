#include <stdlib.h>

#include <gtk/gtk.h>

#include "elpea-main-window.h"



int main (int argc, char *argv[])
{
	g_thread_init (NULL);
	gtk_init (&argc, &argv);
	gtk_gl_init (&argc, &argv);

	GtkWidget *window = elpea_main_window_new ();

	g_signal_connect (G_OBJECT (window), "delete_event",
	                  G_CALLBACK (gtk_main_quit), NULL);
	
	gtk_widget_show (window);

	gtk_main ();

	return EXIT_SUCCESS;
}

