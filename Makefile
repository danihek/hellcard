VERSION = 0.0.1

CFLAGS = -Wall -Wextra
LDFLAGS = -lm

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: hellcard

clean:
	rm -f hellcard

install:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp -p hellcard $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)/hellcard

release: hellcard
	tar czf hellcard-v$(VERSION).tar.gz hellcard

.PHONY: all clean install release uninstall
