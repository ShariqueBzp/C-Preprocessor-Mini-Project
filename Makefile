CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude

TARGET = mypreprocessor

SRC = src/utils.c src/macro.c src/comments.c src/includes.c \
      src/expand.c src/preprocessor.c src/main.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET) tests/demo.c -I. -v
	@echo "----- tests/demo.i -----"
	@cat tests/demo.i

clean:
	rm -f $(OBJ) $(TARGET) tests/demo.i
