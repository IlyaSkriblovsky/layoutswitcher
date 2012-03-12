#include "SwitcherButton.h"

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <gconf/gconf-client.h>

#include <libosso.h>

#include "TabletModel.h"

#define GCONF_DIR_2008      "/apps/osso/inputmethod/hildon-im-languages"
#define GCONF_CURRENT_2008  "/apps/osso/inputmethod/hildon-im-languages/current"
#define GCONF_LANG0_2008    "/apps/osso/inputmethod/hildon-im-languages/language-0"
#define GCONF_LANG1_2008    "/apps/osso/inputmethod/hildon-im-languages/language-1"

#define GCONF_DIR_2006      "/apps/osso/inputmethod"
#define GCONF_CURRENT_2006   "/apps/osso/inputmethod/current_language"
#define GCONF_LANG0_2006    "/apps/osso/inputmethod/primary_language"
#define GCONF_LANG1_2006    "/apps/osso/inputmethod/secondary_language"

#define GCONF_LOCK_DIR   "/system/osso/af"
#define GCONF_LOCK_ENTRY   "/system/osso/af/keyboard-attached"

#define CMD_BLOCK_2006_1  "maemo-gtk-im-switch osso-input-method"
#define CMD_BLOCK_2006_2  "maemo-gtk-im-switch xim"
#define CMD_BLOCK_2006_3  "maemo-gtk-im-switch default"


#define SWITCHER_BUTTON_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE((obj), SWITCHER_BUTTON_TYPE, SwitcherButtonPriv))

struct _SwitcherButtonPriv
{
	TabletModel model;

    osso_context_t* osso;

    int current;
    gboolean locked;

    GtkIconTheme *theme;

    GdkPixbuf *icon0;
    GdkPixbuf *icon1;

    GdkPixbuf *lock;

    GConfClient *gconfClient;
    guint gconfNotifyCurrent;
    guint gconfNotifyLang0;
    guint gconfNotifyLang1;
    guint gconfNotifyLock;

    GtkWidget *menu;
    GtkWidget *menuItemCopy;
    GtkWidget *menuItemBlock;
};

G_DEFINE_TYPE(SwitcherButton, switcher_button, GTK_TYPE_BUTTON);

void switcher_button_dispose(GObject*);
void switcher_button_finalize(GObject*);
void switcher_button_size_request(GtkWidget *self, GtkRequisition *requisition);


// Private methods
void switcher_button_update_icon(SwitcherButton *self);
void switcher_button_load_icon(SwitcherButton *self, gchar *langName, GdkPixbuf **icon, GtkIconTheme *theme);
void switcher_button_update_languages(SwitcherButton *self);

void switcher_button_on_current_changed(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer userData);
void switcher_button_on_lock_changed(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer userData);
void switcher_button_on_language_changed(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer userData);

void switcher_button_on_click(SwitcherButton *self, gpointer data);
void switcher_button_on_tap_and_hold(SwitcherButton *self, gpointer data);

void switcher_button_copy_to_clipboard(SwitcherButton *self);
void switcher_button_block_keyboard(SwitcherButton *self, gboolean block);
void switcher_button_switch_layout(SwitcherButton *self, int langN);

void switcher_button_on_copy_menu(GtkMenuItem* menuItem, gpointer data);
void switcher_button_on_block_menu(GtkMenuItem* menuItem, gpointer data);

gboolean switcher_button_on_expose(SwitcherButton *self, GdkEventExpose *event, gpointer data);


void switcher_button_class_init(SwitcherButtonClass *klass)
{
    GObjectClass *gobjectClass = G_OBJECT_CLASS(klass);
    gobjectClass->dispose = switcher_button_dispose;
    gobjectClass->finalize= switcher_button_finalize;

    GtkWidgetClass *widgetClass = GTK_WIDGET_CLASS(klass);
    widgetClass->size_request = switcher_button_size_request;

    g_type_class_add_private(klass, sizeof(SwitcherButtonPriv));
}

void switcher_button_init(SwitcherButton *self)
{
    self->priv = SWITCHER_BUTTON_GET_PRIVATE(self);

    self->priv->osso = osso_initialize("layoutswitcher", VERSION, 0, 0);

    self->priv->model = tabletModel();

    switch (self->priv->model)
    {
        case N800:
            printf("800\n");
            break;
        case N810:
            printf("810\n");
            break;
        case N900:
            printf("900\n");
            break;
        case N770:
            printf("770\n");
            break;
    }

    self->priv->locked = 0;

    self->priv->theme = gtk_icon_theme_get_default();
    self->priv->lock = gtk_icon_theme_load_icon(self->priv->theme, "layoutswitcher-lock", 40, (GtkIconLookupFlags)0, 0);

    self->priv->icon0 = 0;
    self->priv->icon1 = 0;

    self->priv->gconfClient = gconf_client_get_default();

	if (self->priv->model == N800 || self->priv->model == N810)
	{
		gconf_client_add_dir(self->priv->gconfClient, GCONF_DIR_2008, GCONF_CLIENT_PRELOAD_NONE, 0);

		self->priv->gconfNotifyCurrent = gconf_client_notify_add(self->priv->gconfClient,
				GCONF_CURRENT_2008, switcher_button_on_current_changed, self, 0, 0);
		self->priv->gconfNotifyLang0 = gconf_client_notify_add(self->priv->gconfClient,
				GCONF_LANG0_2008, switcher_button_on_language_changed, self, 0, 0);
		self->priv->gconfNotifyLang1 = gconf_client_notify_add(self->priv->gconfClient,
				GCONF_LANG1_2008, switcher_button_on_language_changed, self, 0, 0);
	}
	else if (self->priv->model == N770)
	{
		gconf_client_add_dir(self->priv->gconfClient, GCONF_DIR_2006, GCONF_CLIENT_PRELOAD_NONE, 0);

		self->priv->gconfNotifyCurrent = gconf_client_notify_add(self->priv->gconfClient,
				GCONF_CURRENT_2006, switcher_button_on_current_changed, self, 0, 0);
		self->priv->gconfNotifyLang0 = gconf_client_notify_add(self->priv->gconfClient,
				GCONF_LANG0_2006, switcher_button_on_language_changed, self, 0, 0);
		self->priv->gconfNotifyLang1 = gconf_client_notify_add(self->priv->gconfClient,
				GCONF_LANG1_2006, switcher_button_on_language_changed, self, 0, 0);
	}

    gconf_client_add_dir(self->priv->gconfClient, GCONF_LOCK_DIR, GCONF_CLIENT_PRELOAD_NONE, 0);
    self->priv->gconfNotifyLock = gconf_client_notify_add(self->priv->gconfClient,
        GCONF_LOCK_ENTRY, switcher_button_on_lock_changed, self, 0, 0);

    self->priv->menu = gtk_menu_new();
    self->priv->menuItemCopy = gtk_menu_item_new_with_label("Copy text");
    self->priv->menuItemBlock= gtk_check_menu_item_new_with_label("Block keyboard");
    g_signal_connect(G_OBJECT(self->priv->menuItemCopy), "activate", G_CALLBACK(switcher_button_on_copy_menu), self);
    g_signal_connect(G_OBJECT(self->priv->menuItemBlock), "activate", G_CALLBACK(switcher_button_on_block_menu), self);
    gtk_menu_shell_append(GTK_MENU_SHELL(self->priv->menu), self->priv->menuItemCopy);
    gtk_menu_shell_append(GTK_MENU_SHELL(self->priv->menu), self->priv->menuItemBlock);
    gtk_widget_show_all(self->priv->menu);

    g_signal_connect(G_OBJECT(self), "clicked", G_CALLBACK(switcher_button_on_click), 0);

    if (self->priv->model == N770)
    {
        g_signal_connect(G_OBJECT(self), "tap-and-hold", G_CALLBACK(switcher_button_on_tap_and_hold), 0);
        gtk_widget_tap_and_hold_setup(GTK_WIDGET(self), 0, 0, 0);
    }
    else
    {
    //    g_signal_connect(G_OBJECT(self), "tap-and-hold", G_CALLBACK(switcher_button_on_tap_and_hold), 0);
        gtk_widget_tap_and_hold_setup(GTK_WIDGET(self), self->priv->menu, 0, 0);
    }

    g_signal_connect_after(G_OBJECT(self), "expose-event", G_CALLBACK(switcher_button_on_expose), 0);

    switcher_button_update_languages(self);
}

void switcher_button_dispose(GObject *gobject)
{
    SwitcherButton *self = SWITCHER_BUTTON(gobject);

    if (self->priv->gconfClient)
    {
        gconf_client_notify_remove(self->priv->gconfClient, self->priv->gconfNotifyCurrent);
        gconf_client_notify_remove(self->priv->gconfClient, self->priv->gconfNotifyLang0);
        gconf_client_notify_remove(self->priv->gconfClient, self->priv->gconfNotifyLang1);

        if (self->priv->model == N800 || self->priv->model == N810)
            gconf_client_remove_dir(self->priv->gconfClient, GCONF_DIR_2008, 0);
        else if (self->priv->model == N770)
            gconf_client_remove_dir(self->priv->gconfClient, GCONF_DIR_2006, 0);

        gconf_client_notify_remove(self->priv->gconfClient, self->priv->gconfNotifyLock);
        gconf_client_remove_dir(self->priv->gconfClient, GCONF_LOCK_DIR, 0);

        g_object_unref(self->priv->gconfClient);
        self->priv->gconfClient = 0;
    }

    if (self->priv->menu)
    {
        g_object_unref(self->priv->menu);
        g_object_unref(self->priv->menuItemCopy);
        g_object_unref(self->priv->menuItemBlock);
        self->priv->menu = 0;
    }

    if (self->priv->osso)
    {
        osso_deinitialize(self->priv->osso);
        self->priv->osso = 0;
    }

    G_OBJECT_CLASS(switcher_button_parent_class)->dispose(gobject);
}

void switcher_button_finalize(GObject *gobject)
{
    SwitcherButton *self = SWITCHER_BUTTON(gobject);

    G_OBJECT_CLASS(switcher_button_parent_class)->finalize(gobject);
}

GtkWidget* switcher_button_new()
{
    return GTK_WIDGET(g_object_new(SWITCHER_BUTTON_TYPE, NULL));
}


//////////////////////////////////////////////////

void switcher_button_update_icon(SwitcherButton *self)
{
    if (self->priv->model == N800 || self->priv->model == N810)
        self->priv->current = gconf_client_get_int(self->priv->gconfClient, GCONF_CURRENT_2008, 0);
    else if (self->priv->model == N770)
        self->priv->current = gconf_client_get_bool(self->priv->gconfClient, GCONF_CURRENT_2006, 0) ? 0 : 1;

    self->priv->locked = gconf_client_get_bool(self->priv->gconfClient, GCONF_LOCK_ENTRY, 0) ? 1 : 0;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(self->priv->menuItemBlock), self->priv->locked != 0);

    gtk_widget_queue_draw(GTK_WIDGET(self));
}

void switcher_button_load_icon(SwitcherButton *self, gchar* langName, GdkPixbuf **icon, GtkIconTheme *theme)
{
    char iconName[50];

    if (langName == 0)
        strcpy(iconName, "layoutswitcher-unk");
    else
    {
        if (strcmp(langName, "") == 0)
            strcpy(iconName, "layoutswitcher-unk");
        else
        {
            strcpy(iconName, "layoutswitcher-");
            strcat(iconName, langName);
        }

        g_free(langName);
    }

    if (*icon != 0)
        g_object_unref(G_OBJECT(*icon));

    *icon = gtk_icon_theme_load_icon(theme, iconName, 40, (GtkIconLookupFlags)0, 0);
}

void switcher_button_update_languages(SwitcherButton *self)
{
	if (self->priv->model == N800 || self->priv->model == N810)
	{
		gchar* lang0 = gconf_client_get_string(self->priv->gconfClient, GCONF_LANG0_2008, 0);
		gchar* lang1 = gconf_client_get_string(self->priv->gconfClient, GCONF_LANG1_2008, 0);

		switcher_button_load_icon(self, lang0, &self->priv->icon0, self->priv->theme);
		switcher_button_load_icon(self, lang1, &self->priv->icon1, self->priv->theme);
	}
    else if (self->priv->model == N770)
    {
		gchar* lang0 = gconf_client_get_string(self->priv->gconfClient, GCONF_LANG0_2006, 0);
		gchar* lang1 = gconf_client_get_string(self->priv->gconfClient, GCONF_LANG1_2006, 0);

		switcher_button_load_icon(self, lang0, &self->priv->icon0, self->priv->theme);
		switcher_button_load_icon(self, lang1, &self->priv->icon1, self->priv->theme);
    }
	else
	{
        self->priv->icon0 = 0;
        self->priv->icon1 = 0;
	}

    switcher_button_update_icon(self);
}

//////////////////////////////////////////////////

void switcher_button_on_current_changed(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer userData)
{
    switcher_button_update_icon(SWITCHER_BUTTON(userData));
}

void switcher_button_on_lock_changed(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer userData)
{
    switcher_button_update_icon(SWITCHER_BUTTON(userData));
}

void switcher_button_on_language_changed(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer userData)
{
    switcher_button_update_languages(SWITCHER_BUTTON(userData));
}

//////////////////////////////////////////////////

void switcher_button_on_selection_requested(GtkClipboard *selection, const gchar* text, gpointer userData)
{
    GtkClipboard *clipboard = gtk_clipboard_get(gdk_atom_intern("CLIPBOARD", FALSE));
    if (text != NULL)
    {
        gtk_clipboard_set_text(clipboard, text, -1);

        SwitcherButton* self = SWITCHER_BUTTON(userData);
        osso_system_note_infoprint(self->priv->osso, "Copied", 0);
    }
}

void switcher_button_copy_to_clipboard(SwitcherButton *self)
{
    GtkClipboard *selection = gtk_clipboard_get(gdk_atom_intern("PRIMARY", FALSE));
    gtk_clipboard_request_text(selection, switcher_button_on_selection_requested, self);
}

void switcher_button_block_keyboard(SwitcherButton *self, gboolean block)
{
    if (self->priv->locked == block)
        return;

    self->priv->locked = block;

    if (self->priv->model == N800 || self->priv->model == N810)
    {
        gconf_client_set_bool(self->priv->gconfClient, GCONF_LOCK_ENTRY, self->priv->locked, 0);
    }

    if (self->priv->model == N770)
    {
        /*
        switch (self->priv->locked)
        {
            case 0:
                g_spawn_command_line_async(CMD_BLOCK_2006_1, 0);
                break;

            case 1:
                g_spawn_command_line_async(CMD_BLOCK_2006_2, 0);
                break;

            case 2:
                g_spawn_command_line_async(CMD_BLOCK_2006_3, 0);
                break;

        }
        */
        if (self->priv->locked)
            g_spawn_command_line_async(CMD_BLOCK_2006_2, 0);
        else
            g_spawn_command_line_async(CMD_BLOCK_2006_1, 0);
    }


    if (self->priv->locked)
        osso_system_note_infoprint(self->priv->osso, "Keyboard blocked", 0);
    else
        osso_system_note_infoprint(self->priv->osso, "Keyboard released", 0);
}

void switcher_button_switch_layout(SwitcherButton *self, int langN)
{
    if (self->priv->current == langN)
        return;

	if (self->priv->model == N810)
	{
		// Simulate Ctrl+Chr sequence
		GdkDisplay* gdkDisplay = gdk_display_get_default();
		if (gdkDisplay == 0) return;

		Display *dpy = (Display*)gdk_x11_display_get_xdisplay(gdkDisplay);

		if (dpy == 0)
			return;

		KeySym  Control_R_Sym = XStringToKeysym("Control_R");
		KeySym  Multi_key_Sym = XStringToKeysym("Multi_key");
		if (Control_R_Sym == 0 || Multi_key_Sym == 0)
			return;
		KeyCode Control_R = XKeysymToKeycode(dpy, Control_R_Sym);
		KeyCode Multi_key = XKeysymToKeycode(dpy, Multi_key_Sym);
		if (Control_R == 0 || Multi_key == 0)
			return;

		XTestFakeKeyEvent(dpy, Control_R, True, CurrentTime);
		XTestFakeKeyEvent(dpy, Multi_key, True, CurrentTime);
		XTestFakeKeyEvent(dpy, Multi_key, False, CurrentTime);
		XTestFakeKeyEvent(dpy, Control_R, False, CurrentTime);
	}

	if (self->priv->model == N800)
		gconf_client_set_int(self->priv->gconfClient, GCONF_CURRENT_2008, langN, 0);

	if (self->priv->model == N770)
		gconf_client_set_bool(self->priv->gconfClient, GCONF_CURRENT_2006, langN == 0, 0);

	if (self->priv->model == N900)
	{
	}
}

void switcher_button_on_click(SwitcherButton *self, gpointer data)
{
    int newLayout = self->priv->current == 0 ? 1 : 0;
    switcher_button_switch_layout(self, newLayout);
}

void switcher_button_on_tap_and_hold(SwitcherButton *self, gpointer data)
{
    switcher_button_copy_to_clipboard(self);
}

void switcher_button_on_copy_menu(GtkMenuItem* menuItem, gpointer userData)
{
    SwitcherButton* self = SWITCHER_BUTTON(userData);
    switcher_button_copy_to_clipboard(self);
}

void switcher_button_on_block_menu(GtkMenuItem* menuItem, gpointer userData)
{
    SwitcherButton* self = SWITCHER_BUTTON(userData);
    switcher_button_block_keyboard(
        self,
        GTK_CHECK_MENU_ITEM(self->priv->menuItemBlock)->active
    );
}

//////////////////////////////////////////////////

gboolean switcher_button_on_expose(SwitcherButton *self, GdkEventExpose *event, gpointer data)
{
    GdkGC *gc = gdk_gc_new(GTK_WIDGET(self)->window);

    GdkPixbuf *icon = self->priv->current == 0 ? self->priv->icon0 : self->priv->icon1;
    if (icon == 0)
    {
        return TRUE;
    }

    int x = GTK_WIDGET(self)->allocation.x + (GTK_WIDGET(self)->allocation.width - gdk_pixbuf_get_width(icon)) / 2;
    int y = GTK_WIDGET(self)->allocation.y + (GTK_WIDGET(self)->allocation.height - gdk_pixbuf_get_height(icon)) / 2;

    gdk_draw_pixbuf(GTK_WIDGET(self)->window, gc,
        icon,
        0, 0, x, y, -1, -1,
        GDK_RGB_DITHER_NORMAL, 0, 0);

    if (self->priv->locked)
    {
        gdk_draw_pixbuf(GTK_WIDGET(self)->window, gc,
            self->priv->lock,
            0, 0, x, y, -1, -1,
            GDK_RGB_DITHER_NORMAL, 0, 0);
    }

    g_object_unref(gc);

    return TRUE;
}

//////////////////////////////////////////////////


void switcher_button_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
    requisition->width = 40;
    requisition->height = 40;
}
