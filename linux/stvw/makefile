CC  = gcc
CXX = g++

all: stvw

CFLAGS = -g
LFLAGS = -lncurses

SRC = filesource.cpp view.cpp main.cpp conio.cpp hex.cpp ctrl.cpp


OBJS = $(addsuffix .o, $(basename $(SRC)))

#%.o : %.c
#	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

stvw: $(OBJS)
	$(CXX) $(OBJS) -o stvw $(LFLAGS)

clean:
	rm -f *.o stvw

