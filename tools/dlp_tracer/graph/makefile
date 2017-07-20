CC = ~/mingw-w32/bin/i686-w64-mingw32-g++


CC_FLAGS = -x c++ -O3 -fno-rtti -fno-exceptions -fno-common -fshort-enums -pedantic-errors -static -static-libgcc -static-libstdc++

LINK_FLAGS = -L. -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32 -lstdc++ -lsupc++ 

graph.exe: graph.o bmp2png.o lodepng.o libkoolplot.a
	$(CC)  -mwindows -o graph.exe -s graph.o bmp2png.o lodepng.o libkoolplot.a $(LINK_FLAGS)

graph.o: graph.c
	$(CC) $(CC_FLAGS) -c  graph.c -o graph.o

bmp2png.o: bmp2png.c
	$(CC) $(CC_FLAGS) -c  bmp2png.c -o bmp2png.o

lodepng.o: lodepng.cpp
	$(CC) $(CC_FLAGS) -c  lodepng.cpp -o lodepng.o

clean:
	rm -f *.o 
	rm -f graph.exe 
