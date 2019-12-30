#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

CC=gcc
CFLAGS=-Wall -Wextra -g -pthread

default: SortedList.h SortedList.c lab2_list.c
	$(CC) $(CFLAGS) -c -o SortedList.o SortedList.c
	$(CC) $(CFLAGS) -o lab2_list SortedList.o lab2_list.c

build: default

profile: default profiles.sh
	./profiles.sh

tests: default test.sh
	./test.sh

graphs: lab2b_list.csv lab2_list.gp
	rm *.png
	gnuplot lab2_list.gp

FILES=SortedList.h SortedList.c lab2_list.c test.sh profiles.sh profile.out README Makefile lab2_list.gp

TARNAME=lab2b-105093055

dist: tests graphs profile $(FILES)
	tar -czf $(TARNAME).tar.gz $(FILES) *.csv *.png

clean:
	rm -f $(TARNAME).tar.gz lab2_list *.o raw.gperf

.PHONY: profile.out
