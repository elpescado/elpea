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

#ifndef __FOO_PREFS_INI_H__
#define __FOO_PREFS_INI_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "foo-prefs.h"


G_BEGIN_DECLS

#define FOO_TYPE_PREFS_INI \
	(foo_prefs_ini_get_type ())
#define FOO_PREFS_INI(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), FOO_TYPE_PREFS_INI, FooPrefsIni))
#define FOO_PREFS_INI_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), FOO_TYPE_PREFS_INI, FooPrefsIniClass))
#define FOO_IS_PREFS_INI(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), FOO_TYPE_PREFS_INI))
#define FOO_IS_PREFS_INI_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), FOO_TYPE_PREFS_INI))
#define FOO_PREFS_INI_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), FOO_TYPE_PREFS_INI, FooPrefsIniClass))


typedef struct _FooPrefsIni		FooPrefsIni;
typedef struct _FooPrefsIniPrivate		FooPrefsIniPrivate;
typedef struct _FooPrefsIniClass		FooPrefsIniClass;

struct _FooPrefsIni{
	FooPrefs		parent;
	FooPrefsIniPrivate	*priv;
};

struct _FooPrefsIniClass
{
	FooPrefsClass		parent;

	/* Signals */
};



GType
foo_prefs_ini_get_type				(void);

FooPrefsIni*
foo_prefs_ini_new				(void);


G_END_DECLS

#endif
