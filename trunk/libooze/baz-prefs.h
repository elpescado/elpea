#ifndef __BAZ_PREFS_H__
#define __BAZ_PREFS_H__

#include <gtk/gtk.h>
#include "foo-prefs.h"


void
baz_prefs_bind_action (FooPrefs    *prefs,
                       GtkAction   *action,
					   const gchar *key);


void
baz_prefs_bind_widget (FooPrefs    *prefs,
                       GtkWidget   *widget,
                       const gchar *key);


#endif /* __BAZ_PREFS_H__ */
