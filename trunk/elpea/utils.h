#include <gtk/gtk.h>
#include "ooze-cache.h"

gsize get_file_size (const char *path);

GdkPixbuf *
load_pixbuf (OozeCache *cache, const gchar *path);
