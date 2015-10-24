xx: main.c
	gcc -std=gnu99 main.c -o xx

install: xx
	cp xx /usr/local/bin

uninstall:
	rm /usr/local/bin/xx
