all: lcmsrv

CC = ~/mingw-w32/bin/i686-w64-mingw32-gcc
CFLAGS = -std=gnu99
LFLAGS =


lcmsrv: main.o lcmlite.o lcmlite_recv.o commport.o
	$(CC) $(LFLAGS) main.o lcmlite.o lcmlite_recv.o commport.o -o lcmsrv.exe -lws2_32

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

lcmlite.o: lcmlite.c lcmlite.h
	$(CC) $(CFLAGS) -c lcmlite.c

lcmlite_recv.o: lcmlite_recv.c lcmlite_recv.h
	$(CC) $(CFLAGS) -c lcmlite_recv.c

commport.o: commport.c commport.h
	$(CC) $(CFLAGS) -c commport.c

clean:
	rm *.o
	if [ -f lcmsrv.exe ]; then rm lcmsrv.exe; fi;


