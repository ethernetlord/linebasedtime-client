CC=gcc
CFLAGS=-Wall
SHELL=/bin/sh
TARGETS=linebasedtime_client
DESTDIR=/usr/local/bin

.PHONY: all install clean



all: $(TARGETS)

linebasedtime_client: linebasedtime_client.c
		$(CC) $(CFLAGS) -o$@ $^

install:
		strip $(TARGETS) && cp $(TARGETS) $(DESTDIR)/lbtime

clean:
		rm $(TARGETS)
