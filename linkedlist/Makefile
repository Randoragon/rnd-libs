CC=gcc
CFLAGS=-std=c99 -pedantic -Werror
OBJS=RND_LinkedList.o
OUT=librnd_linkedlist.so

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -shared $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) -fpic $^ -o $@

clean:
	rm -f *.o

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all
	cp -f -- RND_LinkedList.h /usr/include/RND_LinkedList.h
	chmod 644 -- /usr/include/RND_LinkedList.h
	cp -f -- $(OUT) /usr/lib/$(OUT)
	chmod 755 -- /usr/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) example.o -lrnd_linkedlist -o a.out
