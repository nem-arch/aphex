
TARGET			= aphex

INCDIR			=
LIBDIR			=
LDFLAGS			=
LIBS			= 
CFLAGS			= -Wall -ggdb -lpanel -lncurses -std=c99 -pedantic

$(TARGET): aphex.c buffer.c view.c
	gcc $^ $(CFLAGS) -o $@

clean:
	rm $(TARGET)

