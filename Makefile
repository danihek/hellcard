VERSION = 0.0.1

CFLAGS = -Wall -Wextra
LDFLAGS = -lm

DESTDIR = /usr/local/bin

hellcard: hellcard.c
	$(CC) $(CFLAGS) hellcard.c -o hellcard $(LDFLAGS)

debug: hellcard.c
	$(CC) $(CFLAGS) -ggdb hellcard.c -o hellcard $(LDFLAGS)

clean:
	rm hellcard

install: hellcard
	mkdir -p $(DESTDIR)
	cp -f hellcard $(DESTDIR)
	chmod 755 $(DESTDIR)/hellcard # chmod u=rwx,g=rx,o=rx

uninstall:
	rm -f $(DESTDIR)/hellcard

release: hellcard
	tar czf hellcard-v$(VERSION).tar.gz hellcard

.PHONY: hellcard debug release clean install uninstall
