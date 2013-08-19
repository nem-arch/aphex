
TARGET			= aphex

CC			= gcc
CFLAGS			= -Wall -ggdb -std=c99 -pedantic -Wno-switch

PREFIX			?= /usr
BINPREFIX		= $(PREFIX)/bin
MANPREFIX		= $(PREFIX)/share/man

$(TARGET): aphex.c buffer.c view.c input.c helpers.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm $(TARGET)

install:
	mkdir -p "$(DESTDIR)$(BINPREFIX)"
	cp -p $(TARGET) "$(DESTDIR)$(BINPREFIX)"
	mkdir -p "$(DESTDIR)$(MANPREFIX)"
	cp -p $(TARGET).1 "$(DESTDIR)$(MANPREFIX)/man1"

uninstall:
	mkdir -p "$(DESTDIR)$(BINPREFIX)/$(TARGET)"

