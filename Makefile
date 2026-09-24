CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
AR = ar
ARFLAGS = rcs

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib
INC_DIR = include

TCPL2_DIR = $(SRC_DIR)/tcpl2
BIN_TCPL2 = $(BIN_DIR)/tcpl2
OBJ_TCPL2 = $(OBJ_DIR)/tcpl2
SRCS_TCPL2 = $(wildcard $(TCPL2_DIR)/*.c)
TARGETS_TCPL2 = $(patsubst $(TCPL2_DIR)/%.c, $(BIN_TCPL2)/%, $(SRCS_TCPL2))

ALL_TARGETS = $(TARGETS_TCPL2)

LIBRARY = $(LIB_DIR)/libstring_utils.a
LIB_OBJ = $(OBJ_DIR)/string_utils.o

all: $(LIBRARY) $(ALL_TARGETS)

$(LIBRARY): $(LIB_OBJ) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $<

$(LIB_OBJ): $(SRC_DIR)/string_utils.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_TCPL2)/%: $(TCPL2_DIR)/%.c $(LIBRARY) | $(BIN_TCPL2)
	$(CC) $(CFLAGS) $< $(LIBRARY) -o $@

$(LIB_DIR) $(OBJ_DIR) $(BIN_TCPL2) $(OBJ_TCPL2):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

.PHONY: all clean