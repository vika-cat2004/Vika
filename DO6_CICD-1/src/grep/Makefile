CC=gcc
CFLAGS=-Wall -Wextra -Werror
TARGET = s21_grep

all: s21_grep test_grep

s21_grep: 
	$(CC) $(CFLAGS) -o $(TARGET) s21_grep.c 

test_grep: 
	sh grep_test.sh

clean:
	rm -rf *.o 
	rm -rf test_grep
	rm -rf s21_grep
cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem s21_grep.c
	cppcheck --enable=all --suppress=missingIncludeSystem s21_grep.h
	
rebuild: clean all		