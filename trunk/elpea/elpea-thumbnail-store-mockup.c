#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gtk/gtk.h>


size_t get_file_size (const char *path)
{
	struct stat s = {0};
	stat (path, &s);
	return s.st_size;
}



void
load_dir (GtkTreeModel *model, const gchar *dirname)
{
	GtkListStore *store = GTK_LIST_STORE (model);

	gtk_list_store_clear (store);

	GDir *dir = g_dir_open (dirname, 0, NULL);
	if (dir == NULL)
		return;

	const gchar *file;
	while (file = g_dir_read_name (dir)) {
		g_print ("Trying to load %s...", file);
		char *path = g_build_filename (dirname, file, NULL);

		GdkPixbuf *pix = gdk_pixbuf_new_from_file (path, NULL);
		if (pix) {
			int width = gdk_pixbuf_get_width (pix);
			int height = gdk_pixbuf_get_height (pix);
			int size = (int) get_file_size (path);

			GdkPixbuf *thumb = gdk_pixbuf_scale_simple (pix, 96, 64, GDK_INTERP_BILINEAR);

			gchar *txt = g_strdup_printf ("<b>%s</b>\n<small>%dx%d pixels\n%d bytes</small>", file, width, height, size);

			GtkTreeIter iter;
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, thumb, 1, txt, -1);

			g_free (txt);
			g_print (" ok\n");
		} else {
			g_print (" fail\n");
		}

		g_free (path);
	}
	g_dir_close (dir);
}


GtkTreeModel *
create_tree_model (void)
{
	GtkListStore *store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);

	load_dir (store, "../gtkglimage");

	return GTK_TREE_MODEL (store);
}


