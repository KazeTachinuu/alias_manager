CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
PREFIX = /usr/local

.PHONY: all clean install

all: am

am: aliasmanager.o main.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c alias_manager.h
	$(CC) $(CFLAGS) -c -o $@ $<

install: am
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 am $(DESTDIR)$(PREFIX)/bin/am

clean:
	rm -f *.o am