#include <hildon-status-bar-lib/hildon-status-bar-item.h>
#include <gtk/gtk.h>

#include "SwitcherButton.h"

void *LayoutSwitcher2006_initialize(HildonStatusBarItem *item, GtkWidget **button);
void  LayoutSwitcher2006_destroy(void *data);
void  LayoutSwitcher2006_update(void *data, gint value1, gint value2, const gchar *str);
int   LayoutSwitcher2006_get_priority(void *data);

void LayoutSwitcher2006_entry(HildonStatusBarPluginFn_st *fn)
{
    if (fn == NULL)
        return;

    fn->initialize = LayoutSwitcher2006_initialize;
    fn->destroy = LayoutSwitcher2006_destroy;
    fn->update = LayoutSwitcher2006_update;
    fn->get_priority = LayoutSwitcher2006_get_priority;
}

struct LayoutSwitcher2006Plugin
{
    HildonStatusBarItem *item;

    GtkWidget *switcher;
};

void* LayoutSwitcher2006_initialize(HildonStatusBarItem *item, GtkWidget **button)
{
    struct LayoutSwitcher2006Plugin *plugin = g_new0(struct LayoutSwitcher2006Plugin, 1);

    plugin->item = item;
    *button = plugin->switcher = switcher_button_new();

    gtk_widget_show_all(*button);

    return plugin;
}

void LayoutSwitcher2006_update(void *data, gint value1, gint value2, const gchar *str)
{
    if (! data)
        return;

    struct LayoutSwitcher2006Plugin *plugin = (struct LayoutSwitcher2006Plugin*)data;

    if (value1 == 0)
        gtk_widget_destroy(GTK_WIDGET(plugin->item));
    else
        gtk_widget_show_all(plugin->switcher);
}

void LayoutSwitcher2006_destroy(void *data)
{
    if (! data)
        return;

    struct LayoutSwitcher2006Plugin *plugin = (struct LayoutSwitcher2006Plugin*)data;
    g_object_unref(plugin->switcher);

    g_free(data);
}

gint LayoutSwitcher2006_get_priority(void *data)
{
    return 42;
}
