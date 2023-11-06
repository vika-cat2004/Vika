#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 10000

struct Flags {
  bool e;  // pattern
  bool i;  // ignore uppercase vs lowercase
  bool v;  // invert match
  bool c;  // output count of matching lines only
  bool l;  // output matching files only
  bool n;  // precede each matching line with a line number
  bool h;  // output matching lines without preceding them by file names
  bool s;  // suppress error messages about nonexistent or unreadable files
  bool f;  // takes regexes from a file
  bool o;  // output the matched parts of a matching line
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