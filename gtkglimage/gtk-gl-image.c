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

#include <gtk/gtk.h>

#include <gtk/gtkgl.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "gtk-gl-image.h"
#include "marshallers.h"


G_DEFINE_TYPE (GtkGlImage, gtk_gl_image, GTK_TYPE_DRAWING_AREA)

enum props {
	PROP_0,

	PROP_PIXBUF,
	PROP_FILE,
	PROP_ZOOM,
	PROP_ROTATION,
	PROP_ANIMATIONS,
	PROP_REFLECTION
};

const double ZOOM_MIN = 0.125;
const double ZOOM_MAX = 8.0;

struct _GtkGlImagePrivate
{
	/* Private members go here */

	/* Image contents */
	GdkPixbuf *pixbuf;  /* Image content pixbuf */
	guint      tex_id;  /* GL texture ID        */
	gdouble    ratio;	/* Image width/height   */

	/* Zoom and rotation */
	gfloat     zoom;
	gint       rotation;
	gboolean   auto_fit;

	/* Eye candy */
	gboolean   animations;	/* Whether animations are enabled              */
	gboolean   reflection;	/* Whether to draw reflection beneath image    */

	/* Scrolling */
	GtkAdjustment *hadjustment;
	GtkAdjustment *vadjustment;
	gulong         hadj_id;
	gulong         vadj_id;

	gfloat         scroll_x;
	gfloat         scroll_y;

	gboolean disposed;
};


#define GTK_GL_IMAGE_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	GTK_TYPE_GL_IMAGE, GtkGlImagePrivate))


static void
update_adjustments (GtkGlImage *self);

static float
calculate_scrollbar_position (GtkAdjustment *adj);


static gdouble
gtk_gl_image_calculate_zoom_fit (GtkGlImage *self, gboolean stretch)
{
	GtkGlImagePrivate *priv = self->priv;
	GtkWidget *widget = GTK_WIDGET (self);

	if (priv->pixbuf == NULL)
		return 1.0;

	if (! GTK_WIDGET_REALIZED (widget))
		return 1.0;

	double rx = (double)widget->allocation.width / (double)gdk_pixbuf_get_width (priv->pixbuf);
	double ry = (double)widget->allocation.height / (double)gdk_pixbuf_get_height (priv->pixbuf);

	double zoom = MIN(rx,ry)*0.9;
	return stretch ? zoom : MIN (zoom, 1.0);
}


GtkWidget*
gtk_gl_image_new (void)
{
	GtkGlImage *self = g_object_new (GTK_TYPE_GL_IMAGE, NULL);


	/* Set up Gdk GL Config */

	GdkGLConfig *glconfig;
	glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE);
	if (glconfig == NULL) {
		g_printerr ("gdk_gl_config_new_by_mode failed\n");
		return NULL;
	}


	/* Enable OpenGL on this widget */
	gtk_widget_set_gl_capability (GTK_WIDGET (self),
	                              glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);

	return GTK_WIDGET (self);
}


static void
gtk_gl_image_init (GtkGlImage *self)
{
	self->priv = GTK_GL_IMAGE_GET_PRIVATE (self);
	GtkGlImagePrivate *priv = self->priv;

	priv->zoom = 1.0f;
	priv->auto_fit = TRUE;

	priv->animations = FALSE;
	priv->reflection = TRUE;

	priv->scroll_x = 0.5f;
	priv->scroll_y = 0.5f;

	priv->disposed = FALSE;
}


static void
init_gl (GtkGlImage *self)
{
	//g_print ("init_gl\n");
	GtkWidget *widget = GTK_WIDGET (self);

	GLfloat w = widget->allocation.width;
	GLfloat h = widget->allocation.height;
 	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, w/h, 1.0f,1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable (GL_TEXTURE_2D);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	//g_print ("sizeof(GLuint)=%d sizeof(guint)=%d\n", sizeof(GLuint), sizeof(guint));
}


static guint
gtk_gl_load_texture (GdkPixbuf *pix)
{
//	g_return_val_if_null (pix, 0);
	//g_print ("gtk_gl_load_texture (pix=%p)\n", pix);

	int bpp = gdk_pixbuf_get_n_channels (pix);	// bytes per pixel
	int w = gdk_pixbuf_get_width (pix);
	int h = gdk_pixbuf_get_height (pix);

	//g_print ("   bpp=%d w=%d h=%d\n", bpp, w, h);

	GLenum texture_format;

	if (bpp == 4) {
		texture_format = GL_RGBA;
	} else if (bpp == 3) {
		texture_format = GL_RGB;
	} else {
		return 0;
	}

	GLuint tex = 0;

	glGenTextures (1, &tex);
	glBindTexture (GL_TEXTURE_2D, tex);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D (GL_TEXTURE_2D,
	              0 /* level of detail */,
	              bpp, w, h,
	              0 /* border */,
				  texture_format, GL_UNSIGNED_BYTE,
				  gdk_pixbuf_get_pixels (pix));

	//g_print ("   tex=%u\n", (unsigned int)tex);
	return (guint) tex;
}


static guint
gtk_gl_load_texture_from_file (const gchar *file)
{
	GdkPixbuf *pix = gdk_pixbuf_new_from_file (file, NULL);
	return gtk_gl_load_texture (pix);
}


static void
redraw (GtkGlImage *self)
{
	//g_print ("redraw\n");

	if (! GTK_WIDGET_REALIZED (self))
		return;

//	GtkGlImagePrivate *priv = self->priv;
	GtkWidget *widget = GTK_WIDGET (self);

	gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
	gdk_window_process_updates (widget->window, FALSE);
}


static void
render_background (GtkGlImage *self)
{
	//g_print ("render_background\n");
	glDisable (GL_DEPTH_TEST);

	GtkWidget *widget = GTK_WIDGET (self);
	GLfloat ratio = (GLfloat)widget->allocation.width / (GLfloat)widget->allocation.height;

	//g_print ("ratio = %f\n", ratio);

	GLfloat w = 4.0f * ratio;
	GLfloat h = 4.0f;

//	glEnable (GL_DITHER);
	glBindTexture (GL_TEXTURE_2D, 0);
	glBegin (GL_QUADS);
		glColor3f (0.0f, 0.0f, 0.0f);
		glVertex3f (-w, 0.0f, -4.0f);
		glVertex3f ( w, 0.0f, -4.0f);
		glColor3f (0.20f, 0.20f, 0.20f);
		glVertex3f ( w, -h, -4.0f);
		glVertex3f (-w, -h, -4.0f);
	glEnd ();
//	glDisable (GL_DITHER);

	glEnable (GL_DEPTH_TEST);
}


static void swap2f (GLfloat *x, GLfloat *y)
{
	GLfloat tmp = *x;
	*x = *y;
	*y = tmp;
}


static void
render (GtkGlImage *self)
{
	//g_print ("render\n");
	GtkGlImagePrivate *priv = self->priv;
	GtkWidget *widget = GTK_WIDGET (self);

	render_background (self);

	if (priv->pixbuf == NULL)
		return;
	
	if (priv->tex_id == 0) {
		/* Load textures */
		priv->tex_id = gtk_gl_load_texture (priv->pixbuf);
	}


	/*
	 * (scale == 1.0 && zoom == 1.0) => (img.height == this.height / 8)
	 */
	GLfloat pw = gdk_pixbuf_get_width (priv->pixbuf);  // picture width
	GLfloat ph = gdk_pixbuf_get_height (priv->pixbuf); // picture height
	GLfloat sw = widget->allocation.width;             // viewport width
	GLfloat sh = widget->allocation.height;            // viewport height

	/* For calculation only */
	GLfloat ppw = pw;
	GLfloat pph = ph;
	if ((priv->rotation / 90) % 2) {
		swap2f (&ppw, &pph);
	}

	GLfloat z = priv->zoom;
	GLfloat a = sw/sh;
	GLfloat b = pw/sh * z;
	GLfloat c = ph/sh * z;

	/* Compute translation vector for scrolling */
	/* TODO: center image when not filling wiewport */
	GLfloat tx = 0.0f;
	GLfloat ty = 0.0f;

	if (b > a)
		tx = -2.0f * (calculate_scrollbar_position (priv->hadjustment) - 0.5f) * (b-a);

	if (c > 1)
		ty = +2.0f * (calculate_scrollbar_position (priv->vadjustment) - 0.5f) * (c-1);

	//TODO: sort this out!
//	GLfloat ax = (calculate_scrollbar_position (priv->hadjustment) - 0.5f) * -4 * priv->zoom;
//	GLfloat ay = (calculate_scrollbar_position (priv->vadjustment) - 0.5f) * 4 * priv->zoom;

//	g_printerr ("render scale=%f hscale=%f ax=%f ay=%f\n",
//	            (float)scale, (float)hscale, (float)ax, (float)ay);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// TODO: uncomment below line
	//glTranslatef (ax, ay, 0.0f);
	glPushMatrix ();	
		//glTranslatef (ax, ay, -8.0f/priv->zoom);
//		glTranslatef (0.0f, 0.0f, -8.0f/priv->zoom);

		/* Draw object */
		glPushMatrix ();
			glRotatef (priv->rotation, 0.0f, 0.0f, -1.0f);

			glBindTexture (GL_TEXTURE_2D, priv->tex_id);
			glBegin (GL_QUADS);
				glColor3f (1.0f, 1.0f, 1.0f);
				glTexCoord2i (1, 1);	glVertex3f ( 1.0f*b + tx, -1.0f*c + ty, -1.0f);
				glTexCoord2i (0, 1);	glVertex3f (-1.0f*b + tx, -1.0f*c + ty, -1.0f);
				glTexCoord2i (0, 0);	glVertex3f (-1.0f*b + tx,  1.0f*c + ty, -1.0f);
				glTexCoord2i (1, 0);	glVertex3f ( 1.0f*b + tx,  1.0f*c + ty, -1.0f);
			glEnd ();

		glPopMatrix ();


		/* Draw reflection */

		if (priv->reflection) {
			g_printerr ("Drawing reflection\n");
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix ();
				glTranslatef (0.0f, -2.1f * c, 0.0f);
				glBegin (GL_QUADS);
					glColor4f (1.0f, 1.0f, 1.0f, 0.0f);
					glTexCoord2f (1.0f, 0.5f);	glVertex3f ( 1.0f*b + tx, -0.25f*c + ty, -1.0f);
					glTexCoord2f (0.0f, 0.5f);	glVertex3f (-1.0f*b + tx, -0.25f*c + ty, -1.0f);
					glColor4f (1.0f, 1.0f, 1.0f, 0.3f);
					glTexCoord2f (0.0f, 1.f);	glVertex3f (-1.0f*b + tx,  1.0f*c + ty, -1.0f);
					glTexCoord2f (1.0f, 1.f);	glVertex3f ( 1.0f*b + tx,  1.0f*c + ty, -1.0f);
				glEnd ();
			glPopMatrix ();
		}

	glPopMatrix ();
/*
	glBegin (GL_QUADS);
		glColor4f (1.0f, 1.0f, 1.0f, 0.5f);
		glVertex3f ( 1.0f, -1.0f, -1.0f);
		glVertex3f (-1.0f, -1.0f, -1.0f);
		glVertex3f (-1.0f,  1.0f, -1.0f);
		glVertex3f ( 1.0f,  1.0f, -1.0f);
	glEnd ();
*/
}


/* Setting/getting images */

void
gtk_gl_image_set_from_pixbuf (GtkGlImage *self,
                              GdkPixbuf  *pixbuf)
{
	//g_print ("gtk_gl_image_set_from_pixbuf\n");
	GtkGlImagePrivate *priv = self->priv;

	if (priv->pixbuf) {
		g_object_unref (priv->pixbuf);
	}

	priv->pixbuf = g_object_ref (pixbuf);
	priv->tex_id = 0;
//	priv->tex_id = gtk_gl_load_texture (priv->pixbuf);

	gdouble w = gdk_pixbuf_get_width (pixbuf);
	gdouble h = gdk_pixbuf_get_height (pixbuf);
	priv->ratio = w/h;

	//g_print (" -> pix = %p\n", priv->pixbuf);
	//g_print (" -> tex = %u\n", priv->tex_id);
	//g_print (" -> ratio = %lf\n", priv->ratio);
	
	g_object_freeze_notify (G_OBJECT (self));

	if (priv->auto_fit) {
		priv->zoom = gtk_gl_image_calculate_zoom_fit (self, FALSE);
		g_object_notify (G_OBJECT (self), "zoom");
	}

	redraw (self);

	g_object_notify (G_OBJECT (self), "pixbuf");
	g_object_thaw_notify (G_OBJECT (self));
}


void
gtk_gl_image_set_from_file (GtkGlImage *self,
                            const gchar *file)
{
	//g_print ("gtk_gl_image_set_from_file ('%s')\n", file);
	GdkPixbuf *pix = gdk_pixbuf_new_from_file (file, NULL);
	gtk_gl_image_set_from_pixbuf (self, pix);
	g_object_unref (G_OBJECT (pix));
	g_object_notify (G_OBJECT (self), "file");
}


GdkPixbuf *
gtk_gl_image_get_pixbuf (GtkGlImage *image)
{
	GtkGlImagePrivate *priv = image->priv;
	return g_object_ref (priv->pixbuf);
}


void
gtk_gl_image_clear (GtkGlImage *self)
{
	g_printerr ("*** Function %s not implemented\n", __FUNCTION__);
}


/* Zoom */

void
gtk_gl_image_set_zoom (GtkGlImage *self,
                       gfloat zoom)
{
	GtkGlImagePrivate *priv = self->priv;
	gfloat old_zoom = priv->zoom;
	priv->zoom = zoom;
	redraw (self);

	update_adjustments (self);

	if (old_zoom != priv->zoom);
		g_object_notify (G_OBJECT (self), "zoom");
}


void
gtk_gl_image_zoom_in (GtkGlImage *self)
{
	GtkGlImagePrivate *priv = self->priv;
	gtk_gl_image_set_zoom (self, priv->zoom * 2.0);
}


void
gtk_gl_image_zoom_out (GtkGlImage *self)
{
	GtkGlImagePrivate *priv = self->priv;
	gtk_gl_image_set_zoom (self, priv->zoom * 0.5);
}


void
gtk_gl_image_zoom_fit (GtkGlImage *self, gboolean stretch)
{
	GtkGlImagePrivate *priv = self->priv;

	if (priv->pixbuf == NULL)
		return;

	gdouble new_zoom = gtk_gl_image_calculate_zoom_fit (self, stretch);
	gtk_gl_image_set_zoom (self, new_zoom);
}



gfloat
gtk_gl_image_get_zoom (GtkGlImage *self)
{
	GtkGlImagePrivate *priv = self->priv;
	return priv->zoom;
}



/* Rotation */

void
gtk_gl_image_set_rotation (GtkGlImage *self,
                           gint angle)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (angle % 90 == 0);

	GtkGlImagePrivate *priv = self->priv;

	/* Normalize angle */
	while (angle >= 360)
		angle -= 360;

	priv->rotation = angle;
	redraw (self);
	g_object_notify (G_OBJECT (self), "rotation");
}



void
gtk_gl_image_rotate_left (GtkGlImage *self)
{
	g_return_if_fail (self != NULL);

	GtkGlImagePrivate *priv = self->priv;
	gtk_gl_image_set_rotation (self, priv->rotation + 270);
}


void
gtk_gl_image_rotate_right (GtkGlImage *self)
{
	g_return_if_fail (self != NULL);

	GtkGlImagePrivate *priv = self->priv;
	gtk_gl_image_set_rotation (self, priv->rotation + 90);
}


gint
gtk_gl_image_get_rotation (GtkGlImage *self)
{
	g_return_val_if_fail (self != NULL, 0);

	GtkGlImagePrivate *priv = self->priv;
	return priv->rotation;
}


/* Eye candy */

void
gtk_gl_image_set_animations (GtkGlImage *self,
	                         gboolean animations)
{
	g_printerr ("*** Function %s not implemented\n", __FUNCTION__);
	g_object_notify (G_OBJECT (self), "animations");
}


gboolean
gtk_gl_image_get_animations (GtkGlImage *self)
{
	GtkGlImagePrivate *priv = self->priv;
	return priv->animations;
}


void
gtk_gl_image_set_reflection (GtkGlImage *self,
                             gboolean reflection)
{
	GtkGlImagePrivate *priv = self->priv;
	priv->reflection = reflection;
	redraw (self);
	g_object_notify (G_OBJECT (self), "reflection");
}


gboolean
gtk_gl_image_get_reflection (GtkGlImage *self)
{
	GtkGlImagePrivate *priv = self->priv;
	return priv->reflection;
}





static void
gtk_gl_image_dispose (GObject *object)
{
	GtkGlImage *self = (GtkGlImage*) object;
	GtkGlImagePrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (gtk_gl_image_parent_class)->dispose (object);
}


/* Overrides */

static void
realize (GtkWidget *widget)
{
	//g_print ("realize\n");

	/* First call super method */
	GtkGlImageClass *klass = GTK_GL_IMAGE_GET_CLASS (widget);
	GtkDrawingAreaClass *parent_class = g_type_class_peek_parent (klass);

	GTK_WIDGET_CLASS (parent_class)->realize (widget);


	/* Realize GtkGlImage */

	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	if (! gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
		g_printerr ("gdk_gl_drawable_gl_begin failed\n");
		return;
	}
	
	/* Init OpenGL */
	init_gl (GTK_GL_IMAGE (widget));

	gdk_gl_drawable_gl_end (gldrawable);
}


static gboolean
configure_event (GtkWidget         *widget,
                 GdkEventConfigure *event)
{
	//g_print ("configure_event\n");
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	GLfloat w = widget->allocation.width;
	GLfloat h = widget->allocation.height;

	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
		return FALSE;

	glViewport (0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (90.0f, w/h, 0.1f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
	/*
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	if (w > h) {
		GLfloat aspect = w / h;
		glFrustum (-aspect, aspect, -10.0, 10.0, 2.0, 60.0);
	} else {
		GLfloat aspect = h / w;
		glFrustum (-10.0, 10.0, -aspect, aspect, 2.0, 60.0);
	}

	glMatrixMode (GL_MODELVIEW);
	*/

	gdk_gl_drawable_gl_end (gldrawable);
	return TRUE;
}


static gboolean
expose_event (GtkWidget      *widget,
              GdkEventExpose *event)
{
	//g_print ("expose_event\n");

	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	if (! gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
		g_printerr ("gdk_gl_drawable_gl_begin failed\n");
		return FALSE;
	}

	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	/* Render */
	render (GTK_GL_IMAGE (widget));

	/* Swap buffers. */
	if (gdk_gl_drawable_is_double_buffered (gldrawable))
		gdk_gl_drawable_swap_buffers (gldrawable);
	else
		glFlush ();


	gdk_gl_drawable_gl_end (gldrawable);
	return TRUE;
}

/*
 * Scrolling
 */
static float
calculate_scrollbar_position (GtkAdjustment *adj)
{
	if (adj == NULL)
		return 0.5f;

	gdouble lower = gtk_adjustment_get_lower (adj);
	gdouble value = gtk_adjustment_get_value (adj);
	gdouble upper = gtk_adjustment_get_upper (adj);
	gdouble psize = gtk_adjustment_get_page_size (adj);
	
	return (float) (value / (upper-psize));
}

static void
adjustments_changed (GtkAdjustment *adj,
                     gpointer user_data)
{
	GtkGlImage *self = GTK_GL_IMAGE (user_data);
	GtkGlImagePrivate *priv = self->priv;

	gdouble lower = gtk_adjustment_get_lower (priv->hadjustment);
	gdouble value = gtk_adjustment_get_value (priv->hadjustment);
	gdouble upper = gtk_adjustment_get_upper (priv->hadjustment);
	gdouble psize = gtk_adjustment_get_page_size (priv->hadjustment);
	
	gdouble pos = value / (upper-psize);

	g_print ("adjustments_changed: %lf %lf %lf %lf (%lf)\n", lower, value, upper, psize, pos);
	redraw (self);
}


static void
update_adjustments (GtkGlImage *self)
{
	g_print ("update_adjustments\n");
	GtkWidget *widget = GTK_WIDGET (self);
	GtkGlImagePrivate *priv = self->priv;

	if (priv->pixbuf == NULL)
		return;

	if (priv->hadjustment == NULL)
		return;

	if (priv->vadjustment == NULL)
		return;
	
	GtkAllocation *a = &(widget->allocation);
	gfloat ww = a->width;
	gfloat cw = gdk_pixbuf_get_width (priv->pixbuf) * priv->zoom;
	gfloat wh = a->height;
	gfloat ch = gdk_pixbuf_get_height (priv->pixbuf) * priv->zoom;

	g_object_freeze_notify (G_OBJECT (priv->hadjustment));
	g_object_freeze_notify (G_OBJECT (priv->vadjustment));

	if ((priv->rotation / 90) % 2) {
		swap2f (&cw, &ch);
	}



	gtk_adjustment_set_lower (priv->hadjustment, 0.0);
	gtk_adjustment_set_lower (priv->vadjustment, 0.0);

	gtk_adjustment_set_page_size (priv->hadjustment, ww);
	gtk_adjustment_set_page_size (priv->vadjustment, wh);

	gtk_adjustment_set_upper (priv->hadjustment, cw);
	gtk_adjustment_set_upper (priv->vadjustment, ch);

	gtk_adjustment_set_value (priv->hadjustment, (cw-ww) * priv->scroll_x);
	gtk_adjustment_set_value (priv->vadjustment, (ch-wh) * priv->scroll_y);
	g_print ("(%f, %f)\n", (float) cw * priv->scroll_x, (float) priv->scroll_y);
	
	g_object_thaw_notify (G_OBJECT (priv->hadjustment));
	g_object_thaw_notify (G_OBJECT (priv->vadjustment));

}


static void 
gtk_gl_image_set_scroll_adjustments (GtkGlImage *self,
                                     GtkAdjustment *hadjustment, 
                                     GtkAdjustment *vadjustment)
{
	GtkGlImagePrivate *priv = self->priv;

	g_print ("gtk_gl_image_set_scroll_adjustments (%p, %p)\n",
			 hadjustment, vadjustment);

	if (priv->hadjustment) {
		g_signal_handler_disconnect (priv->hadjustment, priv->hadj_id);
		g_object_unref (priv->hadjustment);
	}

	if (priv->vadjustment) {
		g_signal_handler_disconnect (priv->vadjustment, priv->vadj_id);
		g_object_unref (priv->vadjustment);
	}


	priv->hadjustment = hadjustment ? g_object_ref (hadjustment) : NULL;
	priv->vadjustment = vadjustment ? g_object_ref (vadjustment) : NULL;

	if (hadjustment) {
		priv->hadj_id = g_signal_connect (G_OBJECT (hadjustment), "value-changed",
		                                  G_CALLBACK (adjustments_changed), self);
	}

	if (vadjustment) {
		priv->vadj_id = g_signal_connect (G_OBJECT (vadjustment), "value-changed",
		                                  G_CALLBACK (adjustments_changed), self);
	}
}


static void
gtk_gl_image_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtk_gl_image_parent_class)->finalize (object);
}

	
static void
gtk_gl_image_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	GtkGlImage* self = GTK_GL_IMAGE (object);
	GtkGlImagePrivate* priv = self->priv;

	switch (property_id) {
		case PROP_PIXBUF:
			g_value_set_object (value, priv->pixbuf);
			break;
			
		case PROP_FILE:
			g_value_set_string (value, NULL); // FIXME
			break;
			
		case PROP_ZOOM:
			g_value_set_double (value, priv->zoom);
			break;
			
		case PROP_ROTATION:
			g_value_set_int (value, priv->rotation);
			break;
			
		case PROP_ANIMATIONS:
			g_value_set_boolean (value, priv->animations);
			break;
			
		case PROP_REFLECTION:
			g_value_set_boolean (value, priv->reflection);
			break;
			
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
gtk_gl_image_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	GtkGlImage* self = GTK_GL_IMAGE (object);
	GtkGlImagePrivate* priv = self->priv;

	switch (property_id) {
		case PROP_PIXBUF:
			gtk_gl_image_set_from_pixbuf (self, g_value_get_object (value));
			break;
			
		case PROP_FILE:
			gtk_gl_image_set_from_file (self, g_value_get_string (value));
			break;
			
		case PROP_ZOOM:
			gtk_gl_image_set_zoom (self, g_value_get_double (value));
			break;
			
		case PROP_ROTATION:
			gtk_gl_image_set_rotation (self, g_value_get_int (value));
			break;
			
		case PROP_ANIMATIONS:
			gtk_gl_image_set_animations (self, g_value_get_boolean (value));
			break;
			
		case PROP_REFLECTION:
			gtk_gl_image_set_reflection (self, g_value_get_boolean (value));
			break;
			
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
gtk_gl_image_class_init (GtkGlImageClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gobject_class->get_property = gtk_gl_image_get_property;
	gobject_class->set_property = gtk_gl_image_set_property;
	gobject_class->dispose = gtk_gl_image_dispose;
	gobject_class->finalize = gtk_gl_image_finalize;

	widget_class->realize = realize;
	widget_class->configure_event = configure_event;
	widget_class->expose_event = expose_event;

	klass->set_scroll_adjustments = gtk_gl_image_set_scroll_adjustments;

	/* Install properties */

	g_object_class_install_property (gobject_class, PROP_PIXBUF,
		g_param_spec_object ("pixbuf",
		                     "Pixbuf to be displayed",
				     "Specifies pixbuf to be shown in this widget",
				     GDK_TYPE_PIXBUF,
				     G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_FILE,
		g_param_spec_string ("file",
		                     "Filename to be displayed",
				     "Specifies image file name to be shown",
				     NULL,
				     G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_ZOOM,
		g_param_spec_double ("zoom",
		                     "Zoom level",
				     "Zoom level",
				     ZOOM_MIN,
				     ZOOM_MAX,
				     1.0,
				     G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_ROTATION,
		g_param_spec_int ("rotation",
			          "Image rotation",
				  "Image rotation in degrees",
				  0,
				  360,
				  0,
				  G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_ANIMATIONS,
		g_param_spec_boolean ("animations",
			              "Toggle animations",
				      "Enable animations",
				      FALSE,
				      G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_REFLECTION,
		g_param_spec_boolean ("reflection",
			              "Toggle reflection",
				      "Enable reflection effect",
				      TRUE,
				      G_PARAM_READWRITE));


	/* Signals */

	widget_class->set_scroll_adjustments_signal =
		g_signal_new ("set-scroll-adjustments",
		              G_OBJECT_CLASS_TYPE (gobject_class), 
					  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
					  G_STRUCT_OFFSET (GtkGlImageClass, set_scroll_adjustments),
					  NULL, NULL,
					  g_cclosure_user_marshal_VOID__OBJECT_OBJECT,
					  G_TYPE_NONE, 2, 
					  GTK_TYPE_ADJUSTMENT,
					  GTK_TYPE_ADJUSTMENT);


	g_type_class_add_private (klass, sizeof (GtkGlImagePrivate));
}

