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
#include "foo-prefs.h"


G_DEFINE_TYPE (FooPrefs, foo_prefs, G_TYPE_OBJECT)

struct _FooPrefsPrivate
{
	/* Private members go here */

	gboolean disposed;
};


#define FOO_PREFS_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	FOO_TYPE_PREFS, FooPrefsPrivate))



static void
foo_prefs_init (FooPrefs *self)
{
	self->priv = FOO_PREFS_GET_PRIVATE (self);
	FooPrefsPrivate *priv = self->priv;

	priv->disposed = FALSE;
}

gint
foo_prefs_get_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         default_value)
{
	g_return_val_if_fail (FOO_IS_PREFS (prefs), 0);
	return FOO_PREFS_GET_CLASS (prefs)->get_int (prefs, key, default_value);
}


gboolean
foo_prefs_get_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     default_value)
{
	g_return_val_if_fail (FOO_IS_PREFS (prefs), FALSE);
	return FOO_PREFS_GET_CLASS (prefs)->get_bool (prefs, key, default_value);
}


gchar *
foo_prefs_get_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *default_value)
{
	g_return_val_if_fail (FOO_IS_PREFS (prefs), NULL);
	return FOO_PREFS_GET_CLASS (prefs)->get_string (prefs, key, default_value);
}


void
foo_prefs_set_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         value)
{
	g_return_if_fail (FOO_IS_PREFS (prefs));
	FOO_PREFS_GET_CLASS (prefs)->set_int (prefs, key, value);
}


void
foo_prefs_set_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     value)
{
	g_return_if_fail (FOO_IS_PREFS (prefs));
	FOO_PREFS_GET_CLASS (prefs)->set_bool (prefs, key, value);
}


void
foo_prefs_set_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *value)
{
	g_return_if_fail (FOO_IS_PREFS (prefs));
	FOO_PREFS_GET_CLASS (prefs)->set_string (prefs, key, value);
}




static void
foo_prefs_dispose (GObject *object)
{
	FooPrefs *self = (FooPrefs*) object;
	FooPrefsPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (foo_prefs_parent_class)->dispose (object);
}


static void
foo_prefs_finalize (GObject *object)
{
	G_OBJECT_CLASS (foo_prefs_parent_class)->finalize (object);
}

	
static void
foo_prefs_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	FooPrefs* self = FOO_PREFS (object);
	FooPrefsPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
foo_prefs_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	FooPrefs* self = FOO_PREFS (object);
	FooPrefsPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
foo_prefs_class_init (FooPrefsClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = foo_prefs_get_property;
	gobject_class->set_property = foo_prefs_set_property;
	gobject_class->dispose = foo_prefs_dispose;
	gobject_class->finalize = foo_prefs_finalize;

	g_type_class_add_private (klass, sizeof (FooPrefsPrivate));
}

