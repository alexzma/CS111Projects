#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

CC=gcc
CFLAGS=-O3 -Wall -Wextra -g

default: simpsh.c
	$(CC) $(CFLAGS) -o simpsh simpsh.c

check: default test.sh
	./test.sh

FILES=simpsh.c test.sh README Makefile report.pdf

TARNAME=lab1-105093055

dist: $(FILES)
	tar -czf $(TARNAME).tar.gz $(FILES)

clean:
	rm -f lab1-105093055.tar.gz simpsh
