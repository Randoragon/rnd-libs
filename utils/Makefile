CC=gcc
CFLAGS=-std=c99 -pedantic -Werror
OBJS=RND_Utils.o
OUT=librnd_utils.a
PREFIX=/usr/local

all: main

main: $(OBJS)
	ar -rc $(OUT) $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -f *.o

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all
	mkdir -p -- ${DESTDIR}${PREFIX}/include  ${DESTDIR}${PREFIX}/lib
	cp -f -- RND_Utils.h ${DESTDIR}${PREFIX}/include
	chmod 644 -- ${DESTDIR}${PREFIX}/include/RND_Utils.h
	cp -f -- $(OUT) ${DESTDIR}${PREFIX}/lib
	chmod 755 -- ${DESTDIR}${PREFIX}/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) example.o -L. -lrnd_utils -o a.out
