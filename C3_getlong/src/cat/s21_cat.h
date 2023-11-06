#ifndef S21_CAT_H
#define S21_CAT_H

#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Flags {
  bool b;  // numbers only non-empty lines
  bool e;  // but also display end-of-line characters as '$'
  bool E;  // the same as '-e' but without implying '-v'
  bool v;  // show non printing characters
  bool n;  // number all output lines
  bool s;  // squeeze multiple adjacent blank lines
  bool t;  // but also display tabs as ^I
  bool T;  // same as '-t' but without '-v'
};
typedef struct Flags Flags;

struct option flags_long[] = {{"number-nonblank", 0, 0, 'b'},
                              {"number", 0, 0, 'n'},
                              {"squeeze-blank", 0, 0, 's'},
                              {0, 0, 0, 0}};

void get_flags(int argc, char *argv[], Flags *flags);
void set_flags(char option, Flags *flags);
void s21_cat(int start_index, int file_count, char *file_path[], Flags *flags);
void print_file(FILE *filename, Flags *flags);
void squeeze_blank(FILE *file, char previous_symbol, char current_symbol,
                   Flags *flags);
void flag_e(char current_symbol, Flags *flags);
void print_chars(char current_symbol, Flags *flags);
void flag_t(Flags *flags, char current_symbol);
void print_cntrl(char current_symbol, Flags *flags);
void print_meta(char current_symbol, Flags *flags);
void flags_nb(char previous_symbol, char current_symbol, Flags *flags,
              int *count_lines);

#endif