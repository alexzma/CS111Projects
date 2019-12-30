#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055
CC=gcc
CFLAGS=-O3 -g -Wall -Wextra

default: lab0.c
	$(CC) $(CFLAGS) -o lab0 lab0.c

run: test.sh
	./test.sh

check: default run

clean:
	rm -f lab0 lab0-105093055.tar.gz

FILES=lab0.c README Makefile backtrace.png breakpoint.png test.sh

lab0-105093055.tar.gz: $(FILES)
	tar -czf $@ $(FILES)

dist: lab0-105093055.tar.gz

.PHONY: default check clean dist run
