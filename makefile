CFLAGS=-std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wvla -Warith-conversion -Wno-builtin-declaration-mismatch -Wno-dangling-else
CPPFLAGS=-DNDEBUG -DVERBOSE #-DLOGGING
LDLIBS=-lsodium
deps=logger.o
.PHONY: all clear clean
all: client server compile_flags.txt usr_creator

client: client.c $(deps) *.h makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(deps) $< -o $@ $(LDLIBS)

server: server.c $(deps) *.h makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(deps) $< -o $@ $(LDLIBS)

compile_flags.txt: makefile
	printf '%s\n' $(CFLAGS) > ./compile_flags.txt


usr_creator: usr_creator.c common.h makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
	./$@

clean: clear
clear:
	$(RM) *.o client server usrlist usrinfo usr_creator
