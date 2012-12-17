NAME=contact_list_cmd

CC=g++                               
CFLAGS= -Wall -pedantic -Wno-long-long -g -c

$(NAME): main.c
	$(CC) $(CFLAGS) csvlib.c -o csvlib.o
	$(CC) $(CFLAGS) main.c -o main.o
	$(CC) csvlib.o main.o -o contact_list_cmd
