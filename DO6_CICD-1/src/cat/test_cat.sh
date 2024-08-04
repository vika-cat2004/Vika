# !/bin/bash

./s21_cat -b test.txt >> s21_cat.txt
cat -b test.txt >> cat.txt
diff -s s21_cat.txt cat.txt
rm s21_cat.txt cat.txt

./s21_cat -n test.txt >> s21_cat.txt #мой тестовый файл запис в s21_cat.txt
cat -n test.txt >> cat.txt #оригинальный кат запис в cat.txt
diff -s s21_cat.txt cat.txt
rm s21_cat.txt cat.txt

./s21_cat -s test.txt >> s21_cat.txt
cat -s test.txt >> cat.txt
diff -s s21_cat.txt cat.txt
rm s21_cat.txt cat.txt

./s21_cat -e test.txt >> s21_cat.txt
cat -e test.txt >> cat.txt
diff -s s21_cat.txt cat.txt
rm s21_cat.txt cat.txt

./s21_cat -t test.txt >> s21_cat.txt
cat -t test.txt >> cat.txt
diff -s s21_cat.txt cat.txt
rm s21_cat.txt cat.txt

