#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

FLAGS=-Wall -Wextra -O3 -g -lmraa -pthread -lm -DBEAGLEBONE

default: lab4b.c
	gcc $(FLAGS) -o lab4b lab4b.c

check: default
	./lab4b

FILES=lab4b.c Makefile README

dist: $(FILES)
	tar -czf lab4b-105093055.tar.gz $(FILES)

clean:
	rm -rf lab4b-105093055.tar.gz lab4b
