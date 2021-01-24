CC=gcc
CFLAGS=-std=c99 -pedantic -Werror
LDFLAGS=-lrnd_linkedlist -lrnd_priorityqueue
OBJS=RND_Game.o
OUT=librnd_game.so
PREFIX=/usr/local

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) -fpic $^ -o $@

clean:
	rm -f *.o

install: CFLAGS += -O3
install: clean all
	mkdir -p ${DESTDIR}${PREFIX}/include  ${DESTDIR}${PREFIX}/lib
	cp -f -- RND_Game.h ${DESTDIR}${PREFIX}/include
	chmod 644 -- ${DESTDIR}${PREFIX}/include/RND_Game.h
	cp -f -- $(OUT) ${DESTDIR}${PREFIX}/lib
	chmod 755 -- ${DESTDIR}${PREFIX}/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) -g -Og -lrnd_game example.o -o a.out
