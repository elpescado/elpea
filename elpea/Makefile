CFLAGS=-Wall `pkg-config --cflags gtk+-2.0 gtkglext-1.0` -g -I../gtkglimage
LDFLAGS=-Wall `pkg-config --libs gtk+-2.0 gtkglext-1.0` -lm -g

OBJECTS=\
	elpea-directory.o \
	elpea-main-window.o \
	elpea-thumbnail-store-mockup.o \
	elpea-thumbnail-view.o \
	elpea-thumbnail.o \
	main.o \
	utils.o \
	../gtkglimage/gtk-gl-image.o

all: elpea

elpea: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS) 
