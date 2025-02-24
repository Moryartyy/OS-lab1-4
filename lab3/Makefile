CC=gcc
CFLAGS=-Wall -g -Iinclude

build: main.o
	$(CC) $(CFLAGS) main.c -o lab3

clean:
	rm -rf *.o
	rm -rf lab3
	rm -rf out.txt

compare: build
	./lab3 > out.txt
	diff out.txt correct.txt
