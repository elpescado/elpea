/*
 * GtkGlImage
 * ----------
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

#ifndef __GTK_GL_IMAGE_H__
#define __GTK_GL_IMAGE_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define GTK_TYPE_GL_IMAGE \
	(gtk_gl_image_get_type ())
#define GTK_GL_IMAGE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_GL_IMAGE, GtkGlImage))
#define GTK_GL_IMAGE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_GL_IMAGE, GtkGlImageClass))
#define GTK_IS_GL_IMAGE(obj) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_GL_IMAGE))
#define GTK_GL_IMAGE_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_GL_IMAGE, GtkGlImageClass))


typedef struct _GtkGlImage		GtkGlImage;
typedef struct _GtkGlImagePrivate		GtkGlImagePrivate;
typedef struct _GtkGlImageClass		GtkGlImageClass;

struct _GtkGlImage{
	GtkDrawingArea		parent;
	GtkGlImagePrivate	*priv;
};

struct _GtkGlImageClass
{
	GtkDrawingAreaClass		parent;

	/* Signals */
};



GType
gtk_gl_image_get_type				(void);

GtkWidget*
gtk_gl_image_new				(void);


/* Setting/getting images */

void
gtk_gl_image_set_from_pixbuf (GtkGlImage *self,
                              GdkPixbuf  *pixbuf);

void
gtk_gl_image_set_from_file (GtkGlImage *self,
                            const gchar *file);

GdkPixbuf *
gtk_gl_image_get_pixbuf (GtkGlImage *image);


void
gtk_gl_image_clear (GtkGlImage *self);


/* Zoom */

void
gtk_gl_image_set_zoom (GtkGlImage *self,
                       gfloat zoom);

void
gtk_gl_image_zoom_in (GtkGlImage *self);

void
gtk_gl_image_zoom_out (GtkGlImage *self);

void
gtk_gl_image_zoom_fit (GtkGlImage *self);

gfloat
gtk_gl_image_get_zoom (GtkGlImage *self);


/* Rotation */

void
gtk_gl_image_set_rotation (GtkGlImage *self,
                           gint angle);


void
gtk_gl_image_rotate_left (GtkGlImage *self);

void
gtk_gl_image_rotate_right (GtkGlImage *self);

gint
gtk_gl_image_get_rotation (GtkGlImage *self);


/* Eye candy */

void
gtk_gl_image_set_animations (GtkGlImage *self,
	                         gboolean animations);

gboolean
gtk_gl_image_get_animations (GtkGlImage *self);

void
gtk_gl_image_set_reflection (GtkGlImage *self,
                             gboolean reflection);

gboolean
gtk_gl_image_get_reflection (GtkGlImage *self);

G_END_DECLS

#endif
