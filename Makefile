
TARGET			= aphex

INCDIR			=
LIBDIR			=
LDFLAGS			=
LIBS			= 
CFLAGS			= -Wall -ggdb -std=c99 -pedantic

$(TARGET): aphex.c buffer.c view.c input.c
	gcc $^ $(CFLAGS) -o $@

clean:
	rm $(TARGET)

