SRC=main.c

CC=/home/lz/sdcc/bin/sdcc
FAMILY=pic14
PROC=16f877a

all: $(SRC:.c=.hex) bin

$(SRC:.c=.hex): $(SRC)
	$(CC) -V --use-non-free -m$(FAMILY) -p$(PROC) $^

bin:
	./hex2bin -m 1000 main.hex

clean:
	rm -f $(SRC:.c=.asm) $(SRC:.c=.cod) $(SRC:.c=.hex) $(SRC:.c=.lst) $(SRC:.c=.o) *.bin

.PHONY: all clean
