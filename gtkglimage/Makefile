include ../Makefile.inc
CFLAGS=-Wall -g $(UC_CFLAGS)
LDFLAGS=-Wall -g $(UC_LDFLAGS)

all: gtk-gl-image-test

marshallers.c: marshallers.list
	glib-genmarshal --body $+ > $@

marshallers.h: marshallers.list
	glib-genmarshal --header $+ > $@

marshallers.o: marshallers.c marshallers.h

gtk-gl-image-test: compat.o gtk-gl-image-test.o gtk-gl-image.o marshallers.o

clean:
	rm -rf *.o gtk-gl-image-test
