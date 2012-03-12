CONTROL_EXTRAS ?= ${wildcard preinst postinst prerm postrm}

deb: build/$(PACKAGE)-$(VERSION).deb

build/$(PACKAGE)-$(VERSION).deb: debian
	mkdir -p build
	echo "2.0" >build/debian-binary
	tar zcf build/control.tar.gz -C debian/ .
	fakeroot tar zcf build/data.tar.gz -C inst/ .
	ar -rc $@ build/debian-binary build/control.tar.gz build/data.tar.gz
	rm -f build/debian-binary
	rm -f build/control.tar.gz
	rm -f build/data.tar.gz

debian: debian/control
ifneq (${CONTROL_EXTRAS},)
	cp ${CONTROL_EXTRAS} $@
endif

debian/control: deb-icon.png inst
	mkdir -p debian
	echo "Package: $(PACKAGE)" >$@
	echo "Version: $(VERSION)" >>$@
	echo "Section: $(SECTION)" >>$@
	echo "Priority: $(PRIORITY)" >>$@
	echo "Architecture: $(ARCHITECTURE)" >>$@
	echo "Depends: $(DEPENDS)" >>$@
	echo "Maintainer: $(MAINTAINER)" >>$@
	echo "Description: $(DESCRIPTION)" >>$@
	echo "Installed-Size: `du -s inst | cut -f1`" >>$@
	echo "Maemo-Icon-26:" >>$@
	openssl enc -base64 <deb-icon.png | sed 's/^/ /' >>$@
#ifndef 770
#	echo "Maemo-Icon-26:" >>$@
#	base64 deb-icon.png | sed 's/^/ /' >>$@
#endif
