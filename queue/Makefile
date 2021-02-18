CC=gcc
CFLAGS=-std=c99 -pedantic -Werror
OBJS=RND_Queue.o
OUT=librnd_queue.so
PREFIX=/usr/local

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
	@mkdir -p ${DESTDIR}${PREFIX}/include  ${DESTDIR}${PREFIX}/lib
	cp -f -- RND_Queue.h ${DESTDIR}${PREFIX}/include
	@chmod 644 -- ${DESTDIR}${PREFIX}/include/RND_Queue.h
	cp -f -- $(OUT) ${DESTDIR}${PREFIX}/lib
	@chmod 755 -- ${DESTDIR}${PREFIX}/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) example.o -L. -lrnd_queue -o a.out
