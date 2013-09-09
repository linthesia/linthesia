# -*- mode: makefile-gmake; coding: utf-8 -*-

all:
	$(MAKE) -C src/libmidi all
	$(MAKE) -C src all

install:
	$(MAKE) -C src install
	-mkdir -p $(DESTDIR)/usr/share/linthesia/graphics
	install -m 644 graphics/*.tga graphics/*.ico $(DESTDIR)/usr/share/linthesia/graphics

.PHONY:clean
clean:
	$(MAKE) -C src/libmidi clean
	$(MAKE) -C src clean

