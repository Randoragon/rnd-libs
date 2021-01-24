CC=gcc
CFLAGS=-std=c99 -pedantic -Werror
OBJS=RND_HashMap.o
OUT=librnd_hashmap.so

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
	cp -f -- RND_HashMap.h /usr/include/RND_HashMap.h
	chmod 644 -- /usr/include/RND_HashMap.h
	cp -f -- $(OUT) /usr/lib/$(OUT)
	chmod 755 -- /usr/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) example.o -lrnd_hashmap -o a.out
