CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
TARGET = mytar
SRC = mytar.c

.PHONY: scp clean

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

scp:
	scp $(SRC) stiborlu@u-pl0.ms.mff.cuni.cz:

clean:
	rm -f $(TARGET)
	rm -f testovani/$(TARGET)

