CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -I./structs/include
LDFLAGS = -L./structs/lib -lstructs

OBJ_DIR = build
BIN_NAME = mylang.exe

SRCS = src/error.c \
	   src/token.c \
	   src/lexer.c \
       src/parser.c \
       src/ast.c \
	   src/semantics.c \
	   src/io.c \
	   src/interpreter.c \
       main.c

OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

make: all

all: setup $(BIN_NAME)

# Crear carpeta de objetos
setup:
	@mkdir -p $(OBJ_DIR)/src

# final executable
$(BIN_NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	@echo "------------------------------------------------------------"
	@echo "MYLANG compiled! Executable: $(BIN_NAME)"
	@echo "------------------------------------------------------------"

# compile modules
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_NAME)
	@echo "------------------------------------------------------------"
	@echo "MYLANG cleaned!"
	@echo "------------------------------------------------------------"

.PHONY: all clean setup
