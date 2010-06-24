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

#include <string.h>

#include <gtk/gtk.h>
#include "foo-prefs-ini.h"

#define GENERAL_GROUP "general"


G_DEFINE_TYPE (FooPrefsIni, foo_prefs_ini, FOO_TYPE_PREFS)

struct _FooPrefsIniPrivate
{
	/* Private members go here */
	GKeyFile *ini;
	GHashTable *watches;

	gboolean disposed;
};


#define FOO_PREFS_INI_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	FOO_TYPE_PREFS_INI, FooPrefsIniPrivate))

static void
foo_prefs_ini_notify (FooPrefsIni *prefs,
                      const gchar *key);


FooPrefsIni*
foo_prefs_ini_new (void)
{
	FooPrefsIni *self = g_object_new (FOO_TYPE_PREFS_INI, NULL);
	return self;
}


static void
foo_prefs_ini_init (FooPrefsIni *self)
{
	self->priv = FOO_PREFS_INI_GET_PRIVATE (self);
	FooPrefsIniPrivate *priv = self->priv;

	priv->ini = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (priv->ini, "config.ini",
	                           G_KEY_FILE_KEEP_COMMENTS,
							   &error);
	if (error) {
		g_printerr ("%d: %s\n", error->code, error->message);
		g_error_free (error);
	}

	priv->watches = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

	priv->disposed = FALSE;
}


static gint
foo_prefs_ini_get_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         default_value)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	GError *error = NULL;
	gint val = g_key_file_get_integer (priv->ini, GENERAL_GROUP, key, &error);
	if (error) {
		//g_printerr ("%d: %s\n", error->code, error->message);
		g_error_free (error);
		return default_value;
	} else {
		return val;
	}
}


static gboolean
foo_prefs_ini_get_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     default_value)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	GError *error = NULL;
	gboolean val = g_key_file_get_boolean (priv->ini, GENERAL_GROUP, key, &error);
	if (error) {
		g_printerr ("get(%s): %d: %s\n", key, error->code, error->message);
		g_error_free (error);
		return default_value;
	} else {
		return val;
	}
}


static gchar *
foo_prefs_ini_get_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *default_value)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	GError *error = NULL;
	gchar* val = g_key_file_get_string (priv->ini, GENERAL_GROUP, key, &error);
	if (error) {
		//g_printerr ("%d: %s\n", error->code, error->message);
		g_error_free (error);
		return g_strdup (default_value);
	} else {
		return val;
	}
}


static void
foo_prefs_ini_set_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         value)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	gint old_val = foo_prefs_ini_get_int (FOO_PREFS (prefs), key, 0);
	g_key_file_set_integer (priv->ini, GENERAL_GROUP, key, value);

	if (old_val != value) {
		foo_prefs_ini_notify (FOO_PREFS_INI (prefs), key);
	}
}


static void
foo_prefs_ini_set_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     value)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	gboolean old_val = foo_prefs_ini_get_bool (FOO_PREFS (prefs), key, FALSE);
	g_key_file_set_boolean (priv->ini, GENERAL_GROUP, key, value);

	if (old_val != value) {
		foo_prefs_ini_notify (FOO_PREFS_INI (prefs), key);
	}
}


static void
foo_prefs_ini_set_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *value)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	gchar *old_val = foo_prefs_ini_get_string (FOO_PREFS (prefs), key, "");
	g_key_file_set_string (priv->ini, GENERAL_GROUP, key, value);

	if (strcmp (old_val, value) != 0) {
		foo_prefs_ini_notify (FOO_PREFS_INI (prefs), key);
	}
	g_free (old_val);
}


/**
 * Notify that key has changed
 **/
static void
foo_prefs_ini_notify (FooPrefsIni *prefs,
                      const gchar *key)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	GList *list = (GList *) g_hash_table_lookup (priv->watches, key);
	for (; list; list = list->next) {
		gpointer *ctx = list->data;

		FooPrefsNotify handler = ctx[0];

		if (handler) {
			handler (FOO_PREFS (prefs), key, ctx[1]);
		}
	}
}


static guint
foo_prefs_ini_add_watch  (FooPrefs       *prefs,
                      const gchar    *key,
                      FooPrefsNotify  handler,
                      gpointer        user_data)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	gpointer *ctx = g_new (gpointer, 2);
	ctx[0] = handler;
	ctx[1] = user_data;

	GList *list = (GList *) g_hash_table_lookup (priv->watches, key);
	list = g_list_prepend (list, ctx);
	g_hash_table_replace (priv->watches, g_strdup (key), list);

	return 1;
}


static void
foo_prefs_ini_sync (FooPrefs *prefs)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(prefs)->priv;

	FILE *f = fopen ("config.ini", "w");
	if (f) {
		gsize len;
		gchar *data = g_key_file_to_data (priv->ini, &len, NULL);
		fwrite (data, 1, len, f);
		fclose (f);
	}

}


static void
foo_prefs_ini_dispose (GObject *object)
{
	FooPrefsIni *self = (FooPrefsIni*) object;
	FooPrefsIniPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (foo_prefs_ini_parent_class)->dispose (object);
}



static void
foo_prefs_ini_finalize (GObject *object)
{
	FooPrefsIniPrivate *priv = FOO_PREFS_INI(object)->priv;

	foo_prefs_ini_sync (FOO_PREFS (object));
	g_key_file_free (priv->ini);

	G_OBJECT_CLASS (foo_prefs_ini_parent_class)->finalize (object);
}

	
static void
foo_prefs_ini_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	FooPrefsIni* self = FOO_PREFS_INI (object);
	FooPrefsIniPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
foo_prefs_ini_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	FooPrefsIni* self = FOO_PREFS_INI (object);
	FooPrefsIniPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
foo_prefs_ini_class_init (FooPrefsIniClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	FooPrefsClass *pclass = FOO_PREFS_CLASS (klass);

	gobject_class->get_property = foo_prefs_ini_get_property;
	gobject_class->set_property = foo_prefs_ini_set_property;
	gobject_class->dispose = foo_prefs_ini_dispose;
	gobject_class->finalize = foo_prefs_ini_finalize;


	pclass->get_int = foo_prefs_ini_get_int;
	pclass->get_bool = foo_prefs_ini_get_bool;
	pclass->get_string = foo_prefs_ini_get_string;
	pclass->set_int = foo_prefs_ini_set_int;
	pclass->set_bool = foo_prefs_ini_set_bool;
	pclass->set_string = foo_prefs_ini_set_string;
	pclass->add_watch = foo_prefs_ini_add_watch;

	g_type_class_add_private (klass, sizeof (FooPrefsIniPrivate));
}

