VERSION = 1.0.1

COMMONICONS = layoutswitcher.png      \
              layoutswitcher-lock.png \
              layoutswitcher-unk.png

ICONS = icons2008/layoutswitcher-da_DK.png   \
        icons2008/layoutswitcher-de_DE.png   \
        icons2008/layoutswitcher-el_GR.png   \
        icons2008/layoutswitcher-en_GB.png   \
        icons2008/layoutswitcher-en_US.png   \
        icons2008/layoutswitcher-es_ES.png   \
        icons2008/layoutswitcher-es_MX.png   \
        icons2008/layoutswitcher-fi_FI.png   \
        icons2008/layoutswitcher-fr_CA.png   \
        icons2008/layoutswitcher-fr_FR.png   \
        icons2008/layoutswitcher-hu_HU.png   \
        icons2008/layoutswitcher-it_IT.png   \
        icons2008/layoutswitcher-nl_NL.png   \
        icons2008/layoutswitcher-no_NO.png   \
        icons2008/layoutswitcher-pl_PL.png   \
        icons2008/layoutswitcher-pt_BR.png   \
        icons2008/layoutswitcher-pt_PT.png   \
        icons2008/layoutswitcher-ru_RU.png   \
        icons2008/layoutswitcher-sv_SE.png



DEFINES = '-DVERSION="$(VERSION)"'
PACKAGES = gtk+-2.0 gconf-2.0 libhildondesktop hildon-1 libosso

all: TestApp libLayoutSwitcher2008.so

TestApp: TestApp.o SwitcherButton.o TabletModel.o
	gcc $^ -o $@ `pkg-config --libs $(PACKAGES)`

libLayoutSwitcher2008.so: Applet2008.o SwitcherButton.o TabletModel.o
	gcc $^ -o $@ -shared `pkg-config --libs $(PACKAGES)`

.c.o:
	gcc $(DEFINES) -O2 $< -o $@ -c `pkg-config --cflags $(PACKAGES)`


clean:
	rm -rf *.o libLayoutSwitcher2008.so TestApp


install: libLayoutSwitcher2008.so LayoutSwitcher2008.desktop
	cp libLayoutSwitcher2008.so $(DESTDIR)/usr/lib/hildon-desktop/
	cp LayoutSwitcher2008.desktop $(DESTDIR)/usr/share/applications/hildon-status-bar/
	cp $(COMMONICONS) $(DESTDIR)/usr/share/icons/hicolor/40x40/hildon/
	cp $(ICONS) $(DESTDIR)/usr/share/icons/hicolor/40x40/hildon/
