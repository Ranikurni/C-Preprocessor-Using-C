
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = mypreprocessor

SOURCES = main.c comment_removal.c file_inclusion.c macro_handler.c

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) *.i test2 test3 test4 test5 test6 test7

