include ../Makefile.inc
CFLAGS=-Wall -g $(UC_CFLAGS) -I../gtkglimage -I../libooze
LDFLAGS=-Wall -g `pkg-config --libs gtkglext-1.0 gtk+-2.0 gconf-2.0 gthread-2.0`

OBJECTS=\
	elpea-directory.o \
	elpea-main-window.o \
	elpea-preferences-window.o \
	elpea-thumbnail-store-mockup.o \
	elpea-thumbnail-view.o \
	elpea-thumbnail.o \
	main.o \
	utils.o \
	../gtkglimage/marshallers.o \
	../gtkglimage/compat.o \
	../gtkglimage/gtk-gl-image.o \
	../libooze/libooze.a

all: elpea

elpea: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS)   $(OBJECTS) 
