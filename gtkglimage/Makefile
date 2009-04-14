CFLAGS=-Wall `pkg-config --cflags gtk+-2.0 gtkglext-1.0` -g
LDFLAGS=-Wall `pkg-config --libs gtk+-2.0 gtkglext-1.0` -g

all: gtk-gl-image-test

marshallers.c: marshallers.list
	glib-genmarshal --body $< > $@

marshallers.h: marshallers.list
	glib-genmarshal --header $< > $@

marshallers.o: marshallers.c marshallers.h

gtk-gl-image-test: gtk-gl-image-test.o gtk-gl-image.o marshallers.o

clean:
	rm -rf *.o gtk-gl-image-test
