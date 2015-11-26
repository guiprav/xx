all: build/xx

clean:
	rm -rf build

install:
	cp build/xx /usr/local/bin

uninstall:
	rm /usr/local/bin/xx

build:
	mkdir build

build/xx: build/main.c
	gcc -std=gnu99 build/main.c -o build/xx

build/main.c: build main.hbs.c
	mohawk main.hbs.c >build/main.c
