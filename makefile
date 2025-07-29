CFLAGS=-std=c17 -Wall -Wextra -Wpedantic -Wconversion -Warith-conversion -Wno-builtin-declaration-mismatch
CPPFLAGS=-DNDEBUG -DVERBOSE #-DLOGGING
LDLIBS=-lsodium
deps=logger.o
.PHONY: all clear
all: client server

client: client.c $(deps) *.h makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(deps) $< -o $@ $(LDLIBS)

server: client.c $(deps) *.h makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(deps) $< -o $@ $(LDLIBS)

clear:
	$(RM) *.o client server
