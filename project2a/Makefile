#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

CC=gcc
CFLAGS=-Wall -Wextra -g -pthread

default: lab2_add.c SortedList.h SortedList.c lab2_list.c
	$(CC) $(CFLAGS) -o lab2_add lab2_add.c
	$(CC) $(CFLAGS) -c -o SortedList.o SortedList.c
	$(CC) $(CFLAGS) -o lab2_list SortedList.o lab2_list.c

build: default

tests: default test.sh test2.sh
	./test.sh
	./test2.sh

graphs: lab2_add.csv lab2_list.csv lab2_add.gp lab2_list.gp
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

FILES=lab2_add.c SortedList.h SortedList.c lab2_list.c test.sh README Makefile test2.sh lab2_add.gp lab2_list.gp

TARNAME=lab2a-105093055

dist: tests graphs $(FILES)
	tar -czf $(TARNAME).tar.gz $(FILES) *.csv *.png

clean:
	rm -f $(TARNAME).tar.gz lab2_add lab2_list *.o
