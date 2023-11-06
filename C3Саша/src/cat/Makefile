#! /bin/bash

all: rebuild

build:
	gcc -Wall -Wextra -Werror -std=c11 s21_cat.c -o ./s21_cat

clear:
	rm -rf ./s21_cat

leaks: clear
	gcc -Wall -Wextra -Werror -std=c11 -fsanitize=address s21_cat.c -o s21_cat

test: rebuild
	chmod 774 ./test.sh
	./test.sh

rebuild: clear build