CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
TARGET = mytar
SRC = mytar.c
NAME = stiborlu
HOST = u-pl0.ms.mff.cuni.cz:

.PHONY: scp clean

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

scp:
	scp $(SRC) $(NAME)@$(HOST):

clean:
	rm -f $(TARGET)
