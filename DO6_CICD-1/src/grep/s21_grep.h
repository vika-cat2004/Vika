#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 10000

struct Flags {
  int e;  // Шаблон
  int i;  // Игнорирует различия регистра
  int v;  // Инвертирует смысл поиска соответствий
  int c;  // Выводит только количество совпадающих строк
  int l;  // Выводит только совпадающие файлы
  int n;  // Предваряет каждую строку вывода номером строки из файла ввода
  int h;  //Выводит совпадающие строки, не предваряя их именами файлов
  int s;  //Подавляет сообщения об ошибках о несуществующих или нечитаемых
          //файлах
  int f;  //Получает регулярные выражения из файла
  int o;  //Печатает только совпадающие (непустые) части совпавшей строки
  char regulars[BUFFER];
  char strings[BUFFER];
  int count_strings;
  int count_lines;
};
typedef struct Flags Flags;

void set_flags(int argc, char *argv[], Flags *flags);
void open_file(int argc, char *argv[], Flags *flags);
void processing(int count_files, char *argv[], FILE *cur_file, Flags *flags);
void precede_output(Flags *flags, char *filename, int count_lines,
                    int files_num);
void output_result(char *buffer, Flags *flags, regex_t reg);
void flag_f(Flags *flags);
void output_substring(char *buffer, regex_t reg);

#endif