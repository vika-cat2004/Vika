#! /bin/bash

all: rebuild

build:
	gcc -Wall -Wextra -Werror -std=c11 s21_grep.c -o s21_grep

clear:
	rm -rf ./s21_grep

leaks: clear
	gcc -Wall -Wextra -Werror -std=c11 -fsanitize=address s21_grep.c -o s21_grep

test: rebuild
	chmod 774 ./test.sh
	./test.sh

rebuild: clear build