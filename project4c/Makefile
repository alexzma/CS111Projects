#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

FLAGS=-Wall -Wextra -O3 -g -lm -lpthread -lssl -lcrypto -lmraa -DBEAGLEBONE

default: lab4c.c
	gcc $(FLAGS) -o lab4c_tcp lab4c.c
	gcc $(FLAGS) -o lab4c_tls lab4c.c

FILES=lab4c.c Makefile README

dist: $(FILES)
	tar -czf lab4c-105093055.tar.gz $(FILES)

clean:
	rm -rf lab4c-105093055.tar.gz lab4c_tcp lab4c_tls
