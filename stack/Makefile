CC=gcc
CFLAGS=-std=c99 -pedantic -Wall
OBJS=RND_Stack.o
OUT=librnd_stack.so
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
	cp -f -- RND_Stack.h ${DESTDIR}${PREFIX}/include
	@chmod 644 -- ${DESTDIR}${PREFIX}/include/RND_Stack.h
	cp -f -- $(OUT) ${DESTDIR}${PREFIX}/lib
	@chmod 755 -- ${DESTDIR}${PREFIX}/lib/$(OUT)

example: example.o
	$(CC) $(CFLAGS) example.o -L. -lrnd_stack -o a.out
