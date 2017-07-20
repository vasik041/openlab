CC = ~/mingw-w32/bin/i686-w64-mingw32-g++

libkoolplot.a: plotdata.o plotstream.o koolplot.o bgi_util.o
	ar -rv libkoolplot.a plotdata.o plotstream.o koolplot.o bgi_util.o

clean:
	rm -f *.o 
	rm -f *.exe 
	rm -f libkoolplot.a

CC_FLAGS = -x c++ -O3 -fno-rtti -fno-exceptions -fno-common -fshort-enums -pedantic-errors -static -static-libgcc -static-libstdc++

LINK_FLAGS = -L. -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32 -lstdc++ -lsupc++ 

plotdata.o: plotdata.cxx plotdata.h
	$(CC) $(CC_FLAGS) -c  plotdata.cxx -o plotdata.o

plotstream.o: plotstream.cxx plotstream.h plotdata.h
	$(CC) $(CC_FLAGS) -c  plotstream.cxx -o plotstream.o

koolplot.o: koolplot.c koolplot.h plotstream.h plotdata.h
	$(CC) $(CC_FLAGS) -c  koolplot.c -o koolplot.o
	
bgi_util.o: bgi_util.cpp bgi_util.h
	$(CC) $(CC_FLAGS) -c  bgi_util.cpp -o bgi_util.o

kplot.o: kplot.c
	$(CC) $(CC_FLAGS) -c  kplot.c -o kplot.o

kplot.exe: kplot.o libkoolplot.a
	$(CC)  -mwindows -o kplot.exe -s kplot.o libkoolplot.a $(LINK_FLAGS)


simple.o: simple.c
	$(CC) $(CC_FLAGS) -c  simple.c -o simple.o

simple.exe: simple.o libkoolplot.a
	$(CC)  -mwindows -o simple.exe -s simple.o libkoolplot.a $(LINK_FLAGS)


all: kplot.exe

