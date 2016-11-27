all: weather

#CC = gcc
CC=/home/lz/olimex/gcc-4.3.3/usr/bin/arm-linux-uclibcgnueabi-gcc
CFLAGS = -I/home/lz/olimex/gcc-4.3.3/usr/include -I/home/lz/olimex/gcc-4.3.3/include --sysroot=/home/lz/olimex/gcc-4.3.3/ -isysroot /home/lz/olimex/gcc-4.3.3 -mtune=arm9tdmi -march=armv4t -mabi=aapcs-linux -msoft-float -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
LFLAGS = --sysroot=/home/lz/olimex/gcc-4.3.3/ -isysroot /home/lz/olimex/gcc-4.3.3 -mtune=arm9tdmi -march=armv4t -mabi=aapcs-linux -msoft-float


weather: main.o data.o http.o sql.o sqlite3.o conio.o port.o calendar.o
	$(CC) $(LFLAGS) main.o http.o data.o sql.o sqlite3.o conio.o port.o calendar.o -o weather -pthread -ldl

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

data.o: data.c
	$(CC) $(CFLAGS) -c data.c

http.o: http.c
	$(CC) $(CFLAGS) -c http.c

port.o: port.c
	$(CC) $(CFLAGS) -c port.c

sql.o: sql.c
	$(CC) $(CFLAGS) -c sql.c

sqlite3.o: sqlite3.c
	$(CC) $(CFLAGS) -c sqlite3.c

conio.o: conio.c
	$(CC) $(CFLAGS) -c conio.c

calendar.o: calendar.c
	$(CC) $(CFLAGS) -c calendar.c


clean:
	rm *.o
	if [ -f weather ]; then rm weather; fi;


