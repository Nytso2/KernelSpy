CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses

SRC = src/main.c src/cpu.c src/network.c src/memory.c src/disk.c
OBJ = $(SRC:.c=.o)

BIN = KernelSpy

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

install: $(BIN)
	sudo install -Dm755 $(BIN) /usr/local/bin/$(BIN)

uninstall:
	sudo rm -f /usr/local/bin/$(BIN)
