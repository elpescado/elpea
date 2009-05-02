CFLAGS=-Wall `pkg-config --cflags gtk+-2.0 gtkglext-1.0 gthread-2.0` -g -I../gtkglimage -I../libooze
LDFLAGS=-Wall `pkg-config --libs gtk+-2.0 gtkglext-1.0 gthread-2.0` -lm -g

OBJECTS=\
	elpea-directory.o \
	elpea-main-window.o \
	elpea-thumbnail-store-mockup.o \
	elpea-thumbnail-view.o \
	elpea-thumbnail.o \
	main.o \
	utils.o \
	../gtkglimage/marshallers.o \
	../gtkglimage/compat.o \
	../gtkglimage/gtk-gl-image.o \
	../libooze/ooze-cache.o \
	../libooze/ooze-stree.o

all: elpea

elpea: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS) 
