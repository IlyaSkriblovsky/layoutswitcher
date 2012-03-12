#ifndef __APPLET2008_H
#define __APPLET2008_H

#include <glib-object.h>
#include <libhildondesktop/statusbar-item.h>

G_BEGIN_DECLS

/* Common struct types declarations */
typedef struct _Applet2008      Applet2008;
typedef struct _Applet2008Class Applet2008Class;
typedef struct _Applet2008Priv  Applet2008Priv;

/* Common macros */
#define APPLET2008_TYPE            (applet2008_get_type ())
#define APPLET2008(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), APPLET2008_TYPE, Applet2008))
#define APPLET2008_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  APPLET2008_TYPE, Applet2008Class))
#define IS_APPLET2008(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), APPLET2008_TYPE))
#define IS_APPLET2008_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  APPLET2008_TYPE))
#define APPLET2008_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  APPLET2008_TYPE, Applet2008Class))

/* Instance struct */
struct _Applet2008
{
    StatusbarItem parent;

    Applet2008Priv *priv;
};

/* Class struct */
struct _Applet2008Class
{
    StatusbarItemClass parent_class;
};

GType  applet2008_get_type  (void);

G_END_DECLS

#endif

