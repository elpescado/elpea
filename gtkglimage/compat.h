/* Compatibility functions */

#include <gtk/gtk.h>

#if !GTK_CHECK_VERSION(2,14,0)
gdouble             gtk_adjustment_get_lower            (GtkAdjustment *adjustment);
gdouble             gtk_adjustment_get_page_increment   (GtkAdjustment *adjustment);
gdouble             gtk_adjustment_get_page_size        (GtkAdjustment *adjustment);
gdouble             gtk_adjustment_get_step_increment   (GtkAdjustment *adjustment);
gdouble             gtk_adjustment_get_upper            (GtkAdjustment *adjustment);
void                gtk_adjustment_set_lower            (GtkAdjustment *adjustment,
                                                         gdouble lower);
void                gtk_adjustment_set_page_increment   (GtkAdjustment *adjustment,
                                                         gdouble page_increment);
void                gtk_adjustment_set_page_size        (GtkAdjustment *adjustment,
                                                         gdouble page_size);
void                gtk_adjustment_set_step_increment   (GtkAdjustment *adjustment,
                                                         gdouble step_increment);
void                gtk_adjustment_set_upper            (GtkAdjustment *adjustment,
                                                         gdouble upper);
#endif

#if !GTK_CHECK_VERSION(2,18,0)
void                gtk_widget_set_visible              (GtkWidget *widget,
                                                         gboolean visible);
#endif
