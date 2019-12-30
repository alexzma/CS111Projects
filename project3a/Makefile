#NAME: Alexander Ma
#ID: 105093055
#EMAIL: alexanderzma@hotmail.com
CC=gcc
FLAGS=-Wextra -Wall -g -O3

default: lab3a.c ext2_fs.h
	$(CC) $(FLAGS) lab3a.c -o lab3a

FILES=Makefile README lab3a.c ext2_fs.h

NAME=lab3a-105093055

dist: $(FILES)
	tar -czf $(NAME).tar.gz $(FILES)

clean:
	rm -rf $(NAME).tar.gz lab3a
