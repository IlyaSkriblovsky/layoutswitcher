#include "Applet2008.h"

#include <libhildondesktop/libhildondesktop.h>

#include "SwitcherButton.h"

#define APPLET2008_GET_PRIVATE(x) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((x), APPLET2008_TYPE, Applet2008Priv))

HD_DEFINE_PLUGIN (Applet2008,
                  applet2008,
                  STATUSBAR_TYPE_ITEM);


struct _Applet2008Priv
{
    GtkWidget *switcher;
};

void applet2008_dispose(GObject *self);

void applet2008_class_init (Applet2008Class *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = applet2008_dispose;
    g_type_class_add_private(klass, sizeof(Applet2008Priv));
}


void applet2008_init(Applet2008 *self)
{
    self->priv = APPLET2008_GET_PRIVATE (self);

    self->priv->switcher = switcher_button_new();
    gtk_container_add(GTK_CONTAINER(self),  self->priv->switcher);

    gtk_widget_show_all(GTK_WIDGET(self));
}

void applet2008_dispose(GObject *gobject)
{
    Applet2008 *self = APPLET2008(gobject);

    if (self->priv->switcher)
    {
        g_object_unref(self->priv->switcher);
        self->priv->switcher = 0;
    }
}
