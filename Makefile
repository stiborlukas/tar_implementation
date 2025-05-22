CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
TARGET = mytar
SRC = mytar.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
