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

#include <gtk/gtk.h>
#include "ooze-cache.h"


G_DEFINE_TYPE (OozeCache, ooze_cache, G_TYPE_OBJECT)

struct _OozeCachePrivate
{
	/* Private members go here */
	GHashTable     *dict;

	gsize    max_size;
	gint     max_items;

	gboolean disposed;
};


#define OOZE_CACHE_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	OOZE_TYPE_CACHE, OozeCachePrivate))


OozeCache*
ooze_cache_new (void)
{
	OozeCache *self = g_object_new (OOZE_TYPE_CACHE, NULL);
	return self;
}


static void
ooze_cache_init (OozeCache *self)
{
	self->priv = OOZE_CACHE_GET_PRIVATE (self);
	OozeCachePrivate *priv = self->priv;

	priv->disposed = FALSE;


	priv->dict = g_hash_table_new_full (g_str_hash,
	                                    g_str_equal,
										g_free,
										g_object_unref);
}


void
ooze_cache_add (OozeCache   *self,
                const gchar *key,
                GObject     *value,
				gsize        size)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (key != NULL);

	OozeCachePrivate *priv = self->priv;

	g_hash_table_replace (priv->dict, g_strdup (key), g_object_ref (value));
}

GObject *
ooze_cache_get (OozeCache   *self,
                const gchar *key)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (key != NULL, NULL);

	OozeCachePrivate *priv = self->priv;

	gpointer val = g_hash_table_lookup (priv->dict, key);
	if (val)
		return g_object_ref (val);
	else
		return NULL;
}


void
ooze_cache_delete (OozeCache   *self,
                   const gchar *key)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (key != NULL);

	OozeCachePrivate *priv = self->priv;

	g_hash_table_remove (priv->dict, key);
}


gsize
ooze_cache_get_max_size (OozeCache *self)
{
	g_return_val_if_fail (self != NULL, 0);

	OozeCachePrivate *priv = self->priv;

	return priv->max_size;
}

void
ooze_cache_set_max_size (OozeCache *self,
                         gsize      max_size)
{
	g_return_if_fail (self != NULL);

	OozeCachePrivate *priv = self->priv;

	priv->max_size = max_size;
}

gint
ooze_cache_get_max_items (OozeCache *self)
{
	g_return_val_if_fail (self != NULL, 0);

	OozeCachePrivate *priv = self->priv;

	return priv->max_items;
}

void
ooze_cache_set_max_items (OozeCache *self,
                          gint       max_items)
{
	g_return_if_fail (self != NULL);

	OozeCachePrivate *priv = self->priv;

	priv->max_items = max_items;
}


/* GObject stuff */


static void
ooze_cache_dispose (GObject *object)
{
	OozeCache *self = (OozeCache*) object;
	OozeCachePrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (ooze_cache_parent_class)->dispose (object);
}


static void
ooze_cache_finalize (GObject *object)
{
	OozeCache *self = OOZE_CACHE (object);
	OozeCachePrivate *priv = self->priv;

	g_hash_table_unref (priv->dict);

	G_OBJECT_CLASS (ooze_cache_parent_class)->finalize (object);
}

	
static void
ooze_cache_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	OozeCache* self = OOZE_CACHE (object);
	OozeCachePrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
ooze_cache_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	OozeCache* self = OOZE_CACHE (object);
	OozeCachePrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
ooze_cache_class_init (OozeCacheClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = ooze_cache_get_property;
	gobject_class->set_property = ooze_cache_set_property;
	gobject_class->dispose = ooze_cache_dispose;
	gobject_class->finalize = ooze_cache_finalize;

	g_type_class_add_private (klass, sizeof (OozeCachePrivate));
}

