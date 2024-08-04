#ifndef S21_CAT_H
#define S21_CAT_H
#include <getopt.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} CatInfo;

void open_file(FILE *file);
CatInfo read_flags(int argc, char *argv[], int *invalid);
void number(FILE *file, CatInfo flag);
void symbol_e(FILE *file);
void symbol_E(FILE *file);
void visible(FILE *file);
void tab_t(FILE *file);
void tab_T(FILE *file);
void cat(FILE *file, CatInfo flag);
void file(int argc, char *argv[], CatInfo flag);

#endif