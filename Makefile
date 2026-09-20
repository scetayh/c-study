CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
AR = ar
ARFLAGS = rcs

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib
INC_DIR = include

CPL_DIR = $(SRC_DIR)/cpl
BIN_CPL = $(BIN_DIR)/cpl
OBJ_CPL = $(OBJ_DIR)/cpl
SRCS_CPL = $(wildcard $(CPL_DIR)/*.c)
TARGETS_CPL = $(patsubst $(CPL_DIR)/%.c, $(BIN_CPL)/%, $(SRCS_CPL))

ALL_TARGETS = $(TARGETS_CPL)

LIBRARY = $(LIB_DIR)/libstring_utils.a
LIB_OBJ = $(OBJ_DIR)/string_utils.o

all: $(LIBRARY) $(ALL_TARGETS)

$(LIBRARY): $(LIB_OBJ) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $<

$(LIB_OBJ): $(SRC_DIR)/string_utils.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_CPL)/%: $(CPL_DIR)/%.c $(LIBRARY) | $(BIN_CPL)
	$(CC) $(CFLAGS) $< $(LIBRARY) -o $@

$(LIB_DIR) $(OBJ_DIR) $(BIN_CPL) $(OBJ_CPL):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

.PHONY: all clean