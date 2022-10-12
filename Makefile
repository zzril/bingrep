NAME=bingrep
MAIN=main
LIB_NAME=$(NAME)

SRC_DIR=./src
BIN_DIR=./bin
LIB_DIR=./lib

LIB_SRC_DIR=$(LIB_DIR)/src
LIB_BIN_DIR=$(LIB_DIR)/bin

SRC=$(SRC_DIR)/$(MAIN).c
BIN=$(BIN_DIR)/$(NAME)

LIB_SRC=$(LIB_SRC_DIR)/$(LIB_NAME).c
LIB_BIN=$(LIB_BIN_DIR)/$(LIB_NAME).so

HEADERS=$(LIB_SRC_DIR)/$(LIB_NAME).h

# --------

CC=clang

CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=-L $(LIB_BIN_DIR) -l$(LIB_NAME)

LIB_CFLAGS=$(CFLAGS) -shared -fPIC
#LIB_LD_FLAGS=$(CFLAGS) # no additional libraries used atm

RM=rm

# --------

all: lib main

clean:
	$(RM) ./$(BIN) ./$(LIB_BIN) || true

main: $(BIN)

lib: $(LIB_BIN)

# --------

$(BIN): $(SRC) $(LIB_BIN) $(HEADERS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LIB_BIN)

$(LIB_BIN): $(LIB_SRC) $(HEADERS)
	mkdir -p $(LIB_BIN_DIR)
	$(CC) $(LIB_CFLAGS) -o $@ $<


