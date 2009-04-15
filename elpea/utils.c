#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

gsize get_file_size (const char *path)
{
	struct stat s = {0};
	stat (path, &s);
	return s.st_size;
}



GdkPixbuf *
load_pixbuf (OozeCache *cache, const gchar *path)
{
	if (cache == NULL)
		return gdk_pixbuf_new_from_file (path, NULL);

	GdkPixbuf *pix = (GdkPixbuf *) ooze_cache_get (cache, path);
	if (pix == NULL) {
		pix = gdk_pixbuf_new_from_file (path, NULL);
		if (pix)
			ooze_cache_add (cache, path, G_OBJECT (pix), 1);
	} else {
		g_print ("Pixbuf '%s' loaded from cache;-)\n", path);
	}
	return pix;
}

