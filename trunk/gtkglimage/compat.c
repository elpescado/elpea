/* Compatibility functions */

#include "compat.h" 

#if !GTK_CHECK_VERSION(2,14,0)
gdouble             gtk_adjustment_get_lower            (GtkAdjustment *adjustment)
{
	g_return_val_if_fail (GTK_IS_ADJUSTMENT (adjustment), 0.0);
	return adjustment->lower;
}


gdouble             gtk_adjustment_get_page_increment   (GtkAdjustment *adjustment)
{
	g_return_val_if_fail (GTK_IS_ADJUSTMENT (adjustment), 0.0);
	return adjustment->page_increment;
}


gdouble             gtk_adjustment_get_page_size        (GtkAdjustment *adjustment)
{
	g_return_val_if_fail (GTK_IS_ADJUSTMENT (adjustment), 0.0);
	return adjustment->page_size;
}


gdouble             gtk_adjustment_get_step_increment   (GtkAdjustment *adjustment)
{
	g_return_val_if_fail (GTK_IS_ADJUSTMENT (adjustment), 0.0);
	return adjustment->step_increment;
}


gdouble             gtk_adjustment_get_upper            (GtkAdjustment *adjustment)
{
	g_return_val_if_fail (GTK_IS_ADJUSTMENT (adjustment), 0.0);
	return adjustment->upper;
}


void                gtk_adjustment_set_lower            (GtkAdjustment *adjustment,
                                                         gdouble lower)
{
	g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
	adjustment->lower = lower;
}


void                gtk_adjustment_set_page_increment   (GtkAdjustment *adjustment,
                                                         gdouble page_increment)
{
	g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
	adjustment->page_increment = page_increment;
}


void                gtk_adjustment_set_page_size        (GtkAdjustment *adjustment,
                                                         gdouble page_size)
{
	g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
	adjustment->page_size = page_size;
}


void                gtk_adjustment_set_step_increment   (GtkAdjustment *adjustment,
                                                         gdouble step_increment)
{
	g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
	adjustment->step_increment = step_increment;
}


void                gtk_adjustment_set_upper            (GtkAdjustment *adjustment,
                                                         gdouble upper)
{
	g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
	adjustment->upper = upper;
}


#endif
