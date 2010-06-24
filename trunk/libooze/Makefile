include ../Makefile.inc
CFLAGS=-Wall -g $(UC_CFLAGS)
LDFLAGS=-Wall -g $(UC_LDFLAGS)

all: libooze.a

libooze.a: ooze-cache.o ooze-stree.o foo-prefs.o foo-prefs-gconf.o foo-prefs-ini.o baz-prefs.o
	ar rcs $@ $^


clean:
	rm -rf *.o gtk-gl-image-test
