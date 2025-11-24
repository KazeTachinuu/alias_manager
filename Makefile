VERSION = 2.3.1
CC ?= gcc
CFLAGS ?= -Wall -Wextra -pedantic -std=c11
CFLAGS += -DVERSION='"$(VERSION)"'
LDFLAGS ?=
PREFIX ?= /usr/local
DESTDIR =

.PHONY: all clean install uninstall test

all: am

test: am
	./test.sh

am: alias_manager.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c alias_manager.h
	$(CC) $(CFLAGS) -c -o $@ $<

install: am
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 am $(DESTDIR)$(PREFIX)/bin/am
	install -d $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 644 am.1 $(DESTDIR)$(PREFIX)/share/man/man1/am.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/am
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/am.1

clean:
	rm -f *.o am
