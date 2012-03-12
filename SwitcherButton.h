#ifndef __SWITCHER_BUTTON_H
#define __SWITCHER_BUTTON_H

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define SWITCHER_BUTTON_TYPE            (switcher_button_get_type())
#define SWITCHER_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), SWITCHER_BUTTON_TYPE, SwitcherButton))
#define SWITCHER_BUTTON_CLASS           (G_TYPE_CHECK_CLASS_CAST((klass), SWITCHER_BUTTON_TYPE, SwitcherButtonClass))
#define IS_SWITCHER_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), SWITCHER_BUTTON_TYPE))
#define IS_SWITCHER_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SWITCHER_BUTTON_TYPE))
#define SWITCHER_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), SWITCHER_BUTTON_TYPE, SwitcherButtonClass))


typedef struct _SwitcherButton      SwitcherButton;
typedef struct _SwitcherButtonClass SwitcherButtonClass;
typedef struct _SwitcherButtonPriv  SwitcherButtonPriv;

struct _SwitcherButton
{
    GtkButton parent_instance;

    SwitcherButtonPriv *priv;
};

struct _SwitcherButtonClass
{
    GtkButtonClass parent_class;
};

GType switcher_button_get_type(void);

GtkWidget* switcher_button_new(void);


G_END_DECLS

#endif
