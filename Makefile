
TARGET			= aphex

CC			= gcc
CFLAGS			= -Wall -ggdb -std=c99 -pedantic

PREFIX			?= /usr
BINPREFIX		= $(PREFIX)/bin

$(TARGET): aphex.c buffer.c view.c input.c helpers.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm $(TARGET)

install:
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -p $(TARGET) "$(DESTDIR)$(BINPREFIX)"

uninstall:
	mkdir -p "$(DESTDIR)$(BINPREFIX)/$(TARGET)"

