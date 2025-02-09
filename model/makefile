CC = gcc
CFLAGS = -g -Wall -ansi -pedantic -pthread

all: clean directories server

directories:
	mkdir ./lib/
	mkdir ./obj/

otherlib: picohttpparser.o server_functs.o socket_lib.o
	ar rcs ./lib/libourlibrary.a ./obj/*.o
	ranlib ./lib/libourlibrary.a

picohttpparser.o: ./srclib/picohttpparser.c
	$(CC) -o ./obj/picohttpparser.o -c ./srclib/picohttpparser.c

socket_lib.o: ./srclib/socket_lib.c
	$(CC) $(CFLAGS) -o ./obj/socket_lib.o -c ./srclib/socket_lib.c

server_functs.o: ./srclib/server_functs.c
	$(CC) $(CFLAGS) -o ./obj/server_functs.o -c ./srclib/server_functs.c

server.o: ./src/server.c
	$(CC) $(CFLAGS) -o ./obj/server.o -c ./src/server.c

server: server.o otherlib
	$(CC) $(CFLAGS) -o server ./obj/server.o -L./lib/ -lourlibrary

clean:
	rm -rf ./lib/
	rm -rf ./obj/
	rm -rf server
