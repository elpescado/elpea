/*
 * elpea
 * -----
 *
 * Copyright (C) 2009 Przemysław Sitek
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

#ifndef __OOZE_CACHE_H__
#define __OOZE_CACHE_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define OOZE_TYPE_CACHE \
	(ooze_cache_get_type ())
#define OOZE_CACHE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OOZE_TYPE_CACHE, OozeCache))
#define OOZE_CACHE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), OOZE_TYPE_CACHE, OozeCacheClass))
#define OOZE_IS_CACHE(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OOZE_TYPE_CACHE)
#define OOZE_IS_CACHE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), OOZE_TYPE_CACHE))
#define OOZE_CACHE_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), OOZE_TYPE_CACHE, OozeCacheClass))


typedef struct _OozeCache		OozeCache;
typedef struct _OozeCachePrivate		OozeCachePrivate;
typedef struct _OozeCacheClass		OozeCacheClass;

struct _OozeCache{
	GObject		parent;
	OozeCachePrivate	*priv;
};

struct _OozeCacheClass
{
	GObjectClass		parent;

	/* Signals */
};



GType
ooze_cache_get_type				(void);

OozeCache*
ooze_cache_new				(void);


void
ooze_cache_add (OozeCache   *self,
                const gchar *key,
                GObject     *value,
				gsize        size);

GObject *
ooze_cache_get (OozeCache   *self,
                const gchar *key);


void
ooze_cache_delete (OozeCache   *self,
                   const gchar *key);


gsize
ooze_cache_get_max_size (OozeCache *self);

void
ooze_cache_set_max_size (OozeCache *self,
                         gsize      max_size);

gint
ooze_cache_get_max_items (OozeCache *self);

void
ooze_cache_set_max_items (OozeCache *self,
                          gint       max_items);


G_END_DECLS

#endif
