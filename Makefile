CC = gcc
AR = ar

CPPFLAGS = -Iinclude -MMD -MP -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -Wextra -std=c23 -g
ARFLAGS = rcs

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib

LIBUTILS = $(LIB_DIR)/libutils.a

UTILS_OBJ = \
	$(OBJ_DIR)/string_utils.o

TCPL2_SRC = $(wildcard $(SRC_DIR)/tcpl2/*.c)
TCPL2_BIN = $(patsubst $(SRC_DIR)/tcpl2/%.c,$(BIN_DIR)/tcpl2/%,$(TCPL2_SRC))

DEP_FILES = $(UTILS_OBJ:.o=.d)


.PHONY: all lib tcpl2 clean

all: $(LIBUTILS) $(TCPL2_BIN)

lib: $(LIBUTILS)

tcpl2: $(TCPL2_BIN)


# ------------------------------------------------------------
# Library
# ------------------------------------------------------------

$(LIBUTILS): $(UTILS_OBJ)
	@mkdir -p $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^


# ------------------------------------------------------------
# Object files
# ------------------------------------------------------------

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


# ------------------------------------------------------------
# TCPL exercises
# ------------------------------------------------------------

$(BIN_DIR)/tcpl2/%: $(SRC_DIR)/tcpl2/%.c $(LIBUTILS)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LIBUTILS) -o $@


# ------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------

-include $(DEP_FILES)


# ------------------------------------------------------------
# Clean
# ------------------------------------------------------------

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(BIN_DIR)/tcpl2