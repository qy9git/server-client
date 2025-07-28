deps=logger.o
CPPFLAGS=-DNDEBUG -DVERBOSE #-DLOGGING
LDFLAGS=$(deps)
LDLIBS=-lsodium
CFLAGS=-std=c17 -Wall -Wextra -Wpedantic -Wconversion -Warith-conversion -Wno-builtin-declaration-mismatch
.PHONY: all clear
all: clear $(deps) client server

clear:
	$(RM) *.o client server
