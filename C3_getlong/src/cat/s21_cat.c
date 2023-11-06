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

int main(int argc, char *argv[]) {
  Flags flags = {0};
  get_flags(argc, argv, &flags);
  s21_cat(optind, argc - optind, argv, &flags);
  return 0;
}

void get_flags(int argc, char *argv[], Flags *flags) {
  int flag_index = 0;
  char current_option =
      getopt_long(argc, argv, "+ETtensbv", flags_long, &flag_index);
  while (current_option != -1) {
    set_flags(current_option, flags);
    current_option =
        getopt_long(argc, argv, "+ETtensbv", flags_long, &flag_index);
  }
  if (flags->b) {
    flags->n = false;
  }
}

void set_flags(char option, Flags *flags) {
  switch (option) {
    case 'b':
      flags->b = true;
      break;
    case 'e':
      flags->e = true;
      flags->v = true;
      break;
    case 'E':
      flags->e = true;
      break;
    case 'n':
      flags->n = true;
      break;
    case 's':
      flags->s = true;
      break;
    case 't':
      flags->v = true;
      flags->t = true;
      break;
    case 'T':
      flags->t = true;
      break;
    case 'v':
      flags->v = true;
      break;
    default:
      fprintf(stderr, "Usage cat [OPTION] [FILE]...");
      exit(1);
  }
}

void s21_cat(int start_index, int file_count, char *file_path[], Flags *flags) {
  for (int i = start_index; i < start_index + file_count; i++) {
    FILE *current_file = fopen(file_path[i], "r");
    if (current_file == NULL) {
      fprintf(stderr, "cat: %s: No such file or directory\n", file_path[i]);
    } else {
      print_file(current_file, flags);
    }
    fflush(stdout);
    if (current_file != NULL) {
      fclose(current_file);
    }
  }
}

void print_file(FILE *filename, Flags *flags) {
  char previous_symbol = '\n';
  char current_symbol = fgetc(filename);
  int count_lines = 0;
  while (!feof(filename)) {
    flags_nb(previous_symbol, current_symbol, flags, &count_lines);
    flag_e(current_symbol, flags);
    print_chars(current_symbol, flags);
    squeeze_blank(filename, previous_symbol, current_symbol, flags);
    previous_symbol = current_symbol;
    current_symbol = fgetc(filename);
  }
}

void squeeze_blank(FILE *file, char previous_symbol, char current_symbol,
                   Flags *flags) {
  if (flags->s && previous_symbol == '\n' && current_symbol == '\n') {
    current_symbol = fgetc(file);
    while (current_symbol == '\n') {
      current_symbol = fgetc(file);
    }
    ungetc(current_symbol, file);
  }
}

void flag_e(char current_symbol, Flags *flags) {
  if (flags->e && current_symbol == '\n') {
    printf("$");
  }
}

void flags_nb(char previous_symbol, char current_symbol, Flags *options,
              int *count_lines) {
  if (options->b && previous_symbol == '\n' && current_symbol != '\n') {
    fprintf(stdout, "%6u\t", ++(*count_lines));
  } else if (options->n && previous_symbol == '\n') {
    fprintf(stdout, "%6u\t", ++(*count_lines));
  }
}

void print_chars(char current_symbol, Flags *flags) {
  if (isprint(current_symbol)) {
    printf("%c", current_symbol);
  } else if (current_symbol == '\t') {
    flag_t(flags, current_symbol);
  } else if (current_symbol == '\n') {
    printf("%c", current_symbol);
  } else if (iscntrl(current_symbol)) {
    print_cntrl(current_symbol, flags);
  } else {
    print_meta(current_symbol, flags);
  }
}

void flag_t(Flags *flags, char current_symbol) {
  if (flags->t) {
    fputc('^', stdout);
    fputc(64 + current_symbol, stdout);
  } else {
    fputc(current_symbol, stdout);
  }
}

void print_cntrl(char current_symbol, Flags *flags) {
  if (flags->v) {
    fputc('^', stdout);
    if (current_symbol == 127) {
      printf("%c", current_symbol - 64);
    } else {
      printf("%c", current_symbol + 64);
    }
  } else {
    printf("%c", current_symbol);
  }
}

void print_meta(char current_symbol, Flags *flags) {
  if (flags->v) {
    char meta_symbol = (signed char)current_symbol + 128;
    if (isprint(meta_symbol)) {
      printf("%c", meta_symbol);
    } else {
      print_cntrl(meta_symbol, flags);
    }
  } else {
    printf("%c", current_symbol);
  }
}
