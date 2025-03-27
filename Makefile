CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/main.c src/execute.c src/utils.c
OBJ = obj/main.o obj/execute.o obj/utils.o
BIN = bin/myshell

# Ensure directories exist before compiling
$(BIN): $(OBJ) | bin obj
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

# Compile each source file into an object file
obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories if they don't exist
bin:
	mkdir -p bin

obj:
	mkdir -p obj

# Run test scripts
test: $(BIN)
	./tests/test_script.sh

# Clean build files
clean:
	rm -rf obj bin
