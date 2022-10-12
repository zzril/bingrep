NAME=bingrep

SRC=$(NAME).c
BIN=$(NAME)

CC=clang
CFLAGS=-Wall -Wextra -pedantic

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -o $@ $^


