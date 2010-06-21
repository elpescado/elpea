CFLAGS=-Wall `pkg-config --cflags gtk+-2.0 gtkglext-1.0 gconf-2.0` -g
LDFLAGS=-Wall `pkg-config --libs gtk+-2.0 gtkglext-1.0 gconf-2.0` -g

all: libooze.a

libooze.a: ooze-cache.o ooze-stree.o foo-prefs.o foo-prefs-gconf.o
	ar rcs $@ $^


clean:
	rm -rf *.o gtk-gl-image-test
