CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
TARGET = mytar
SRC = mytar.c

.PHONY: all clean

all: $(SRC)
	scp $(SRC) stiborlu@u-pl0.ms.mff.cuni.cz:

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	rm -f testovani/$(TARGET)

