CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
TARGET = mytar
SRC = mytar.c
DEST = /testovani/$(TARGET)

.PHONY: all clean

all: $(TARGET)
	# Zkopíruj do cílové složky
	cp $(TARGET) testovani/

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	rm -f testovani/$(TARGET)

