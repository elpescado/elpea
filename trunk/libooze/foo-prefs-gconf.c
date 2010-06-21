/*
 * elpea
 * -----
 *
 * Copyright (C) 2010 Przemysław Sitek
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <gtk/gtk.h>
#include "foo-prefs-gconf.h"

#include <gconf/gconf-client.h>

#define KEY_PATH_MAX 256

G_DEFINE_TYPE (FooPrefsGconf, foo_prefs_gconf, FOO_TYPE_PREFS)

struct _FooPrefsGconfPrivate
{
	/* Private members go here */
	GConfClient *gconf;
	gchar *app_name;

	gboolean disposed;
};


#define FOO_PREFS_GCONF_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	FOO_TYPE_PREFS_GCONF, FooPrefsGconfPrivate))


FooPrefsGconf*
foo_prefs_gconf_new (void)
{
	FooPrefsGconf *self = g_object_new (FOO_TYPE_PREFS_GCONF, NULL);
	return self;
}


static void
foo_prefs_gconf_init (FooPrefsGconf *self)
{
	self->priv = FOO_PREFS_GCONF_GET_PRIVATE (self);
	FooPrefsGconfPrivate *priv = self->priv;

	priv->gconf = gconf_client_get_default ();
	priv->app_name = "elpea";

	priv->disposed = FALSE;
}


static gint
foo_prefs_gconf_get_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         default_value)
{
	FooPrefsGconfPrivate *priv = FOO_PREFS_GCONF(prefs)->priv;
	gchar path[KEY_PATH_MAX];
	snprintf (path, KEY_PATH_MAX, "/apps/%s/%s", priv->app_name, key);

	GError *error = NULL;
	gint value = gconf_client_get_int (priv->gconf, path, &error);
	if (error) {
		g_printerr ("Error: %d: %s\n", error->code, error->message);
		g_error_free (error);
		return default_value;
	} else {
		return value;
	}
}


static gboolean
foo_prefs_gconf_get_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     default_value)
{
	FooPrefsGconfPrivate *priv = FOO_PREFS_GCONF(prefs)->priv;
	gchar path[KEY_PATH_MAX];
	snprintf (path, KEY_PATH_MAX, "/apps/%s/%s", priv->app_name, key);

	GError *error = NULL;
	gboolean value = gconf_client_get_bool (priv->gconf, path, &error);
	if (error) {
		g_printerr ("Error: %d: %s\n", error->code, error->message);
		g_error_free (error);
		return default_value;
	} else {
		return value;
	}
}


static gchar *
foo_prefs_gconf_get_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *default_value)
{
	FooPrefsGconfPrivate *priv = FOO_PREFS_GCONF(prefs)->priv;
	gchar path[KEY_PATH_MAX];
	snprintf (path, KEY_PATH_MAX, "/apps/%s/%s", priv->app_name, key);

	GError *error = NULL;
	gchar * value = gconf_client_get_string (priv->gconf, path, &error);
	if (error) {
		g_printerr ("Error: %d: %s\n", error->code, error->message);
		g_error_free (error);
		return g_strdup (default_value);
	} else {
		return value;
	}
}


static void
foo_prefs_gconf_set_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         value)
{
	FooPrefsGconfPrivate *priv = FOO_PREFS_GCONF(prefs)->priv;
	gchar path[KEY_PATH_MAX];
	snprintf (path, KEY_PATH_MAX, "/apps/%s/%s", priv->app_name, key);

	GError *error = NULL;
	gconf_client_set_int (priv->gconf, path, value, &error);
	if (error) {
		g_printerr ("Error: %d: %s\n", error->code, error->message);
		g_error_free (error);
	}
}


static void
foo_prefs_gconf_set_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     value)
{
	FooPrefsGconfPrivate *priv = FOO_PREFS_GCONF(prefs)->priv;
	gchar path[KEY_PATH_MAX];
	snprintf (path, KEY_PATH_MAX, "/apps/%s/%s", priv->app_name, key);

	GError *error = NULL;
	gconf_client_set_bool (priv->gconf, path, value, &error);
	if (error) {
		g_printerr ("Error: %d: %s\n", error->code, error->message);
		g_error_free (error);
	}
}


static void
foo_prefs_gconf_set_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *value)
{
	FooPrefsGconfPrivate *priv = FOO_PREFS_GCONF(prefs)->priv;
	gchar path[KEY_PATH_MAX];
	snprintf (path, KEY_PATH_MAX, "/apps/%s/%s", priv->app_name, key);

	GError *error = NULL;
	gconf_client_set_string (priv->gconf, path, value, &error);
	if (error) {
		g_printerr ("Error: %d: %s\n", error->code, error->message);
		g_error_free (error);
	}
}





static void
foo_prefs_gconf_dispose (GObject *object)
{
	FooPrefsGconf *self = (FooPrefsGconf*) object;
	FooPrefsGconfPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (foo_prefs_gconf_parent_class)->dispose (object);
}


static void
foo_prefs_gconf_finalize (GObject *object)
{
	G_OBJECT_CLASS (foo_prefs_gconf_parent_class)->finalize (object);
}

	
static void
foo_prefs_gconf_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	FooPrefsGconf* self = FOO_PREFS_GCONF (object);
	FooPrefsGconfPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
foo_prefs_gconf_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	FooPrefsGconf* self = FOO_PREFS_GCONF (object);
	FooPrefsGconfPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
foo_prefs_gconf_class_init (FooPrefsGconfClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	FooPrefsClass *pclass = FOO_PREFS_CLASS (klass);

	gobject_class->get_property = foo_prefs_gconf_get_property;
	gobject_class->set_property = foo_prefs_gconf_set_property;
	gobject_class->dispose = foo_prefs_gconf_dispose;
	gobject_class->finalize = foo_prefs_gconf_finalize;

	pclass->get_int = foo_prefs_gconf_get_int;
	pclass->get_bool = foo_prefs_gconf_get_bool;
	pclass->get_string = foo_prefs_gconf_get_string;
	pclass->set_int = foo_prefs_gconf_set_int;
	pclass->set_bool = foo_prefs_gconf_set_bool;
	pclass->set_string = foo_prefs_gconf_set_string;

	g_type_class_add_private (klass, sizeof (FooPrefsGconfPrivate));
}

