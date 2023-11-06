#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef S21_GREP_H
#define S21_GREP_H

typedef struct Options {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  char reg_exp[10000];
  int err;
} Options;

void parseFlags(int argc, char **argv, Options *grepFlag);
void processFiles(int argc, char **argv, Options grepFlag);
void parseFile(int files_num, char **argv, FILE *file, Options grepFlag);
void fileAndNumberOutput(Options grepFlag, char *filename, int count_lines,
                         int files_num);
void outputResult(char *buffer, Options grepFlag, regex_t reg);
void readRegex(Options *grepFlag);
void outputFlagO(char *buffer, regex_t reg);

#endif