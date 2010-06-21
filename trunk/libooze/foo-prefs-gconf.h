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

#ifndef __FOO_PREFS_GCONF_H__
#define __FOO_PREFS_GCONF_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "foo-prefs.h"


G_BEGIN_DECLS

#define FOO_TYPE_PREFS_GCONF \
	(foo_prefs_gconf_get_type ())
#define FOO_PREFS_GCONF(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), FOO_TYPE_PREFS_GCONF, FooPrefsGconf))
#define FOO_PREFS_GCONF_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), FOO_TYPE_PREFS_GCONF, FooPrefsGconfClass))
#define FOO_IS_PREFS_GCONF(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), FOO_TYPE_PREFS_GCONF))
#define FOO_IS_PREFS_GCONF_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), FOO_TYPE_PREFS_GCONF))
#define FOO_PREFS_GCONF_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), FOO_TYPE_PREFS_GCONF, FooPrefsGconfClass))


typedef struct _FooPrefsGconf		FooPrefsGconf;
typedef struct _FooPrefsGconfPrivate		FooPrefsGconfPrivate;
typedef struct _FooPrefsGconfClass		FooPrefsGconfClass;

struct _FooPrefsGconf{
	FooPrefs		parent;
	FooPrefsGconfPrivate	*priv;
};

struct _FooPrefsGconfClass
{
	FooPrefsClass		parent;

	/* Signals */
};



GType
foo_prefs_gconf_get_type				(void);

FooPrefsGconf*
foo_prefs_gconf_new				(void);


G_END_DECLS

#endif
