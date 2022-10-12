NAME=bingrep
MAIN=main
LIB_NAME=$(NAME)

SRC_DIR=./src
BLD_DIR=./build
BIN_DIR=./bin
LIB_DIR=./lib

LIB_SRC_DIR=$(LIB_DIR)/src
LIB_BLD_DIR=$(LIB_DIR)/shared

LIB_SOURCES=$(LIB_SRC_DIR)/$(LIB_NAME).c
LIB_OBJ_SHARED=$(LIB_BLD_DIR)/$(LIB_NAME).so

SOURCES=$(SRC_DIR)/$(MAIN).c $(LIB_SOURCES)
HEADERS=$(LIB_SRC_DIR)/$(LIB_NAME).h
OBJECTS=$(BLD_DIR)/$(MAIN).o $(BLD_DIR)/$(LIB_NAME).o
BIN=$(BIN_DIR)/$(NAME)

# --------

CC=clang
LD=$(CC)

CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=$(CFLAGS)

LIB_SHARED_FLAGS=$(CFLAGS) -shared -fPIC

RM=rm

# --------

all: main

clean:
	$(RM) ./$(BIN) ./$(OBJECTS) ./$(LIB_OBJ_SHARED) || true

main: $(BIN)

lib: $(LIB_OBJ_SHARED)

# --------

$(BIN): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(BLD_DIR)/$(MAIN).o: $(SRC_DIR)/$(MAIN).c $(HEADERS)
	mkdir -p $(BLD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BLD_DIR)/$(LIB_NAME).o: $(LIB_SRC_DIR)/$(LIB_NAME).c $(HEADERS)
	mkdir -p $(BLD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB_OBJ_SHARED): $(LIB_SOURCES) $(HEADERS)
	mkdir -p $(LIB_BLD_DIR)
	$(CC) $(LIB_SHARED_FLAGS) -o $@ $< $(LIB_SHARED_FLAGS)


