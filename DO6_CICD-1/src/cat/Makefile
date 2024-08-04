CC=gcc
CFLAGS=-Wall -Wextra -Werror

all: s21_cat test_cat

s21_cat: s21_cat.o 
	$(CC) $(CFLAGS) s21_cat.o -o s21_cat

test_cat: s21_cat test_cat.sh
	sh test_cat.sh

clean:
	rm -rf *.o 
	rm -rf s21_cat

rebuild: clean all