CC=gcc
CFLAGS=-std=c99 -pedantic -Werror
OBJS=RND_HashMap.o
OUT=librnd_hashmap.so
PREFIX=/usr/local

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -shared -lrnd_linkedlist $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) -fpic $^ -o $@

clean:
	rm -f *.o

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all
	mkdir -p ${DESTDIR}${PREFIX}/include  ${DESTDIR}${PREFIX}/lib
	cp -f -- RND_HashMap.h ${DESTDIR}${PREFIX}/include
	chmod 644 -- ${DESTDIR}${PREFIX}/include/RND_HashMap.h
	cp -f -- $(OUT) ${DESTDIR}${PREFIX}/lib
	chmod 755 -- ${DESTDIR}${PREFIX}/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) example.o -lrnd_hashmap -o a.out
