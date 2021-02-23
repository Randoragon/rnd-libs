CC=gcc
CFLAGS=-std=c99 -pedantic -Wall
PREFIX=/usr/local

all: main

main:
	@printf "This is just a header file, no need to compile\n"

install:
	@mkdir -p -- ${DESTDIR}${PREFIX}/include
	cp -f -- RND_ErrMsg.h ${DESTDIR}${PREFIX}/include
	@chmod 644 -- ${DESTDIR}${PREFIX}/include/RND_ErrMsg.h

clean:

example: example.o
	$(CC) $(CFLAGS) -L. example.c -o a.out
