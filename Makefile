CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = bank
SRC     = src/main.c src/ui_menu.c src/accounts.c src/transactions.c src/filestore.c

all: $(TARGET)

$(TARGET): $(SRC) include/bank.h
	$(CC) $(CFLAGS) -Iinclude -o $(TARGET) $(SRC)

run: all
	./$(TARGET)

test: all
	bash tests/smoke_test.sh

perf: all
	python3 tests/perf_test.py

clean:
	rm -rf $(TARGET) data/accounts.txt data/transactions.txt data/accounts.tmp data/backup

.PHONY: all run test perf clean
