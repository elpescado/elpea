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

#ifndef __FOO_PREFS_H__
#define __FOO_PREFS_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define FOO_TYPE_PREFS \
	(foo_prefs_get_type ())
#define FOO_PREFS(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), FOO_TYPE_PREFS, FooPrefs))
#define FOO_PREFS_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), FOO_TYPE_PREFS, FooPrefsClass))
#define FOO_IS_PREFS(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), FOO_TYPE_PREFS))
#define FOO_IS_PREFS_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), FOO_TYPE_PREFS))
#define FOO_PREFS_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), FOO_TYPE_PREFS, FooPrefsClass))


typedef struct _FooPrefs		FooPrefs;
typedef struct _FooPrefsPrivate		FooPrefsPrivate;
typedef struct _FooPrefsClass		FooPrefsClass;

typedef void (*FooPrefsNotify) (FooPrefs *, const gchar *, gpointer);

struct _FooPrefs{
	GObject		parent;
	FooPrefsPrivate	*priv;
};

struct _FooPrefsClass
{
	GObjectClass		parent;

	/* Virtual methods */
	gint         (*get_int)    (FooPrefs *, const gchar *, gint);
	gboolean     (*get_bool)   (FooPrefs *, const gchar *, gboolean);
	gchar*       (*get_string) (FooPrefs *, const gchar *, const gchar *);
	void         (*set_int)    (FooPrefs *, const gchar *, gint);
	void         (*set_bool)   (FooPrefs *, const gchar *, gboolean);
	void         (*set_string) (FooPrefs *, const gchar *, const gchar *);
	guint        (*add_watch)  (FooPrefs *, const gchar *, FooPrefsNotify, gpointer);


	/* Signals */
};



GType
foo_prefs_get_type				(void);


gint
foo_prefs_get_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         default_value);


gboolean
foo_prefs_get_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     default_value);


gchar *
foo_prefs_get_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *default_value);


void
foo_prefs_set_int (FooPrefs    *prefs,
                   const gchar *key,
                   gint         value);


void
foo_prefs_set_bool (FooPrefs    *prefs,
                    const gchar *key,
                    gboolean     value);


void
foo_prefs_set_string (FooPrefs    *prefs,
                      const gchar *key,
                      const gchar *value);

                    
guint
foo_prefs_add_watch  (FooPrefs       *prefs,
                      const gchar    *key,
                      FooPrefsNotify  handler,
                      gpointer        user_data);


G_END_DECLS

#endif
