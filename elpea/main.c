#include "../config.h"

#include <stdlib.h>

#include <gtk/gtk.h>

#include "elpea-main-window.h"

#ifdef HAVE_GCONF
#  include "foo-prefs-gconf.h"
#else
#  include "foo-prefs-ini.h"
#endif


FooPrefs *prefs = NULL;

int main (int argc, char *argv[])
{
	g_thread_init (NULL);
	gtk_init (&argc, &argv);
	gtk_gl_init (&argc, &argv);

#ifdef HAVE_GCONF
	prefs = FOO_PREFS (foo_prefs_gconf_new ());
#else
	prefs = FOO_PREFS (foo_prefs_ini_new ());
#endif

	GtkWidget *window = elpea_main_window_new ();

	g_signal_connect (G_OBJECT (window), "delete_event",
	                  G_CALLBACK (gtk_main_quit), NULL);
	
	gtk_widget_show (window);

	gtk_main ();

	g_object_unref (prefs);

	return EXIT_SUCCESS;
}

