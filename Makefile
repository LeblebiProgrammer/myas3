CC = gcc
CFLAGS = -g -Wall

all: wtfServer wtfClient

wtfServer: server.c
	$(CC) $(CFLAGS) -o wtfServer server.c helperFunctions.c

wtfClient: client.c
	$(CC) $(CFLAGS) -o wtfClient client.c helperFunctions.c -lssl -lcrypto

clean:
	rm -f wtfServer.o wtfServer
	rm -f wtfClient.o wtfClient
