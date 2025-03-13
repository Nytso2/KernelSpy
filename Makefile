CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lncurses

SRC = src/main.c src/cpu.c src/network.c src/memory.c src/disk.c src/temp.c
OBJ = $(SRC:.c=.o)
TARGET = KernelSpy

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
