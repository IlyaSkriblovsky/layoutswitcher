#include <gtk/gtk.h>

#include "SwitcherButton.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_POPUP);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), 0);

    GtkWidget* switcher = switcher_button_new();

    gtk_container_add(GTK_CONTAINER(window), switcher);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
