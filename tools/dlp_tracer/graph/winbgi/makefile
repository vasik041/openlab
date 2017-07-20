CXX = ~/mingw-w32/bin/i686-w64-mingw32-g++
AR = ~/mingw-w32/bin/i686-w64-mingw32-ar

libbgi.a: drawing.o misc.o mouse.o palette.o text.o winbgi.o winthread.o file.o dibutil.o winbgim.h graphics.h
	$(AR) -rv libbgi.a drawing.o misc.o mouse.o palette.o text.o winthread.o winbgi.o file.o dibutil.o
clean:
	rm *.o  
	rm libbgi.a

CPPFLAGS = -c -O3 -fno-rtti -fno-exceptions

drawing.o: drawing.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) drawing.cxx

misc.o: misc.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) misc.cxx

mouse.o: mouse.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) mouse.cxx

palette.o: palette.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) palette.cxx

text.o: text.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) text.cxx

winbgi.o: winbgi.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) winbgi.cxx

winthread.o: winthread.cxx winbgim.h winbgitypes.h
	$(CXX) $(CPPFLAGS) winthread.cxx

dibutil.o: dibutil.cxx dibutil.h dibapi.h
	$(CXX) $(CPPFLAGS) dibutil.cxx

file.o: file.cxx dibutil.h dibapi.h winbgitypes.h
	$(CXX) $(CPPFLAGS) -c file.cxx

graphics.h: winbgim.h
	rm graphics.h
	cp winbgim.h graphics.h

bgi.exe: bgi.cxx
	$(CXX) bgi.cxx -o bgi.exe

