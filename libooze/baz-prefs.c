#include "baz-prefs.h"

static void
on_action_toggled (GtkToggleAction *toggleaction,
                   gpointer         user_data)
{
	gpointer *ctx = user_data;
	FooPrefs *prefs = ctx[0];
	const gchar *key = ctx[1];

	foo_prefs_set_bool (prefs, key, gtk_toggle_action_get_active (toggleaction));
}


static void
preferences_changed                  (FooPrefs    *prefs,
                                      const gchar *key,
                                      gpointer     data)
{
	GtkToggleAction *action = GTK_TOGGLE_ACTION (data);
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), foo_prefs_get_bool (prefs, key, FALSE));
}



void free_ctx (gpointer  user_data,
               GClosure *closure)
{
	gpointer *ctx = user_data;
	g_free (ctx[1]);
	g_free (ctx);
}


void
baz_prefs_bind_action (FooPrefs    *prefs,
                       GtkAction   *action,
					   const gchar *key)
{
	gpointer *ctx = g_new0 (gpointer, 2);
	ctx[0] = prefs;
	ctx[1] = g_strdup (key);

	if (GTK_IS_TOGGLE_ACTION (action)) {
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), foo_prefs_get_bool (prefs, key, FALSE));
		g_signal_connect_data (action, "toggled", G_CALLBACK (on_action_toggled), ctx, free_ctx, 0);
		foo_prefs_add_watch (prefs, key, preferences_changed, action);
	}
}
