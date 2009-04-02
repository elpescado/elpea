CFLAGS=`pkg-config --cflags gtk+-2.0 gtkglext-1.0` -g
LDFLAGS=`pkg-config --libs gtk+-2.0 gtkglext-1.0` -g

all: gtk-gl-image-test

gtk-gl-image-test: gtk-gl-image-test.o gtk-gl-image.o

clean:
	rm -rf *.o gtk-gl-image-test
