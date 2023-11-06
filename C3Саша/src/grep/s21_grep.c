#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SUCCESS_EXIT 0
#define ERROR_EXIT 1

struct Options {
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool o;
  bool o_count;
};

struct Patterns {
  char *pattern;
  struct Patterns *ptr;
};

char **parser(int agrc, char **agrv, char **patterns, int *pattern_count,
              struct Options *options, int *exit_status);
char **add_patterns(char option, char **patterns, int *pattern_count,
                    char *optarg, struct Options options, int *exit_status);
void add_option(char option, struct Options *options);
void conflicting_flags(struct Options *options);
size_t getstr(char **str, size_t *n, FILE *f);
void print_module(int agrc, char **agrv, int optind, char **patterns,
                  int pattern_count, struct Options options);
int print_file(char *name, char **patterns, int pattern_count,
               struct Options options);
int print_stdin(char **patterns, int pattern_count, struct Options options);
int print_str(char *str, char **patterns, int pattern_count,
              struct Options options, char *file_name, int j);
int checkfile(char *name, FILE *f, struct Options options,
              bool is_pattern_file);
void print_count(struct Options options, int match_count, char *name);
void print_names(struct Options options, int match_count, char *name);

int main(int agrc, char **agrv) {
  char **patterns = (char **)malloc(1 * sizeof(char *));
  int pattern_count = 0, correct_flags = SUCCESS_EXIT;
  struct Options options = {false};

  if (agrc > 1) {
    patterns =
        parser(agrc, agrv, patterns, &pattern_count, &options, &correct_flags);
    if (!correct_flags) {
      if (pattern_count == 0 && optind < agrc) {
        char *pattern =
            (char *)malloc((strlen(agrv[optind]) + 1) * sizeof(char));
        strcpy(pattern, agrv[optind]);
        patterns[0] = pattern;
        pattern_count++;
        optind++;
      }
      if (agrc - optind <= 1) options.h = true;
      print_module(agrc, agrv, optind, patterns, pattern_count, options);
    }
  } else
    fprintf(stderr, "Usage: grep [OPTION]... PATTERN [FILE]...\n");

  for (int i = 0; i < pattern_count; i++) {
    free(patterns[i]);
  }
  free(patterns);

  return 0;
}

char **parser(int agrc, char **agrv, char **patterns, int *pattern_count,
              struct Options *options, int *exit_status) {
  char option = 0;
  char *optstring = "e:ivclnhsf:o";
  while ((option = getopt(agrc, agrv, optstring)) != -1 && option != ':' &&
         option != '?' && *exit_status == SUCCESS_EXIT) {
    if (option == 'e' || option == 'f')
      patterns = add_patterns(option, patterns, pattern_count, optarg, *options,
                              exit_status);
    else
      add_option(option, options);
  }
  if (option != -1) {
    *exit_status = ERROR_EXIT;
  }
  conflicting_flags(options);
  return patterns;
}

char **add_patterns(char option, char **patterns, int *pattern_count,
                    char *optarg, struct Options options, int *exit_status) {
  if (option == 'e') {
    patterns =
        (char **)realloc(patterns, (*pattern_count + 1) * sizeof(char *));
    char *pattern = (char *)malloc((strlen(optarg) + 1) * sizeof(char));
    strcpy(pattern, optarg);
    patterns[*pattern_count] = pattern;
    (*pattern_count)++;
  } else if (option == 'f') {
    FILE *f = fopen(optarg, "r");

    if (checkfile(optarg, f, options, true))
      *exit_status = ERROR_EXIT;
    else {
      size_t str_length = 0;
      char *temp_str = NULL;
      while ((int)(str_length = getstr(&temp_str, &str_length, f)) != EOF) {
        patterns =
            (char **)realloc(patterns, (*pattern_count + 1) * sizeof(char *));
        patterns[*pattern_count] = temp_str;
        (*pattern_count)++;
        temp_str = NULL;
        str_length = 0;
      }
      free(temp_str);
      fclose(f);
    }
  }

  return patterns;
}

void add_option(char option, struct Options *options) {
  switch (option) {
    case 'i':
      options->i = true;
      break;
    case 'v':
      options->v = true;
      break;
    case 'c':
      options->c = true;
      break;
    case 'l':
      options->l = true;
      break;
    case 'n':
      options->n = true;
      break;
    case 'h':
      options->h = true;
      break;
    case 's':
      options->s = true;
      break;
    case 'o':
      options->o = true;
  }
}

void conflicting_flags(struct Options *options) {
  if (options->l) {
    options->h = true;
    options->n = false;
    options->o = false;
  }
  if (options->c) {
    options->n = false;
    options->o = false;
  }
  if (options->v) options->o = false;
}

size_t getstr(char **str, size_t *n, FILE *f) {
  char *temp = *str;

  if (temp == NULL) {
    *n = 2;
    temp = (char *)malloc(*n * sizeof(char));
  }

  char c;
  size_t i = 0;
  while ((c = getc(f)) != EOF && c != '\n') {
    if (i == *n - 1) {
      *n *= 2;
      temp = (char *)realloc(temp, *n * sizeof(char));
    }
    temp[i] = c;
    i++;
  }
  temp[i] = '\0';

  if (c == EOF && i == 0) i = EOF;
  *str = temp;
  return i;
}

void print_module(int agrc, char **agrv, int optind, char **patterns,
                  int pattern_count, struct Options options) {
  if (optind == agrc)
    print_stdin(patterns, pattern_count, options);
  else {
    for (; optind < agrc; optind++) {
      if (!strcmp(agrv[optind], "-"))
        print_stdin(patterns, pattern_count, options);
      else
        print_file(agrv[optind], patterns, pattern_count, options);
    }
  }
}

int print_file(char *name, char **patterns, int pattern_count,
               struct Options options) {
  FILE *file = fopen(name, "r");

  int match_count = 0;
  if (!checkfile(name, file, options, false)) {
    size_t str_length = 0;
    int i = 1;
    char *str = NULL;
    while ((int)(str_length = getstr(&str, &str_length, file)) != EOF) {
      int status = print_str(str, patterns, pattern_count, options, name, i);
      free(str);
      str = NULL;
      str_length = 0;
      i++;
      if (!status) match_count++;
    }
    free(str);
    print_count(options, match_count, name);
    print_names(options, match_count, name);
    fclose(file);
  }

  return match_count;
}

int print_stdin(char **patterns, int pattern_count, struct Options options) {
  size_t str_length = 0;
  int i = 1, match_count = 0;
  char *name = "(standart input):";
  char *str = NULL;
  while ((int)(str_length = getstr(&str, &str_length, stdin)) != EOF) {
    int status = print_str(str, patterns, pattern_count, options, name, i);
    free(str);
    str = NULL;
    str_length = 0;
    i++;
    if (!status) match_count++;
  }
  free(str);
  print_count(options, match_count, name);
  print_names(options, match_count, name);

  return match_count;
}

int print_str(char *str, char **patterns, int pattern_count,
              struct Options options, char *file_name, int str_count) {
  int mod, status = REG_NOMATCH;
  if (options.i)
    mod = REG_ICASE | REG_EXTENDED;
  else
    mod = REG_EXTENDED;
  size_t nmatch = 1;
  regmatch_t matchptr[nmatch];
  for (int i = 0; i < pattern_count && status != 0; i++) {
    regex_t *compiled = (regex_t *)malloc(8 * sizeof(regex_t *));
    regcomp(compiled, patterns[i], mod);
    int temp_status = regexec(compiled, str, nmatch, matchptr, 0);
    if (!temp_status) status = 0;
    regfree(compiled);
    free(compiled);
  }
  if (options.v) status = !status;
  if (!status && !options.c && !options.l) {
    if (!options.h) printf("%s:", file_name);
    if (options.n) {
      if (options.o && !options.o_count) {
        printf("%d:", str_count);
        options.o_count = true;
      } else if (!options.o)
        printf("%d:", str_count);
    }
    if (!options.o)
      printf("%s\n", str);
    else if (options.o) {
      for (int j = matchptr[0].rm_so; j < matchptr[0].rm_eo; j++) {
        printf("%c", str[j]);
      }
      printf("\n");
      if (matchptr[0].rm_eo < (long long int)strlen(str))
        print_str((str + matchptr[0].rm_eo), patterns, pattern_count, options,
                  file_name, str_count);
      else
        options.o_count = false;
    }
  }

  return status;
}

int checkfile(char *name, FILE *f, struct Options options,
              bool is_pattern_file) {
  int exit_status = SUCCESS_EXIT;
  if (is_pattern_file) options.s = false;

  struct stat buf;
  stat(name, &buf);

  if (f == NULL) {
    if (S_ISREG(buf.st_mode)) {
      exit_status = ERROR_EXIT;
      if (!options.s) fprintf(stderr, "grep: %s: Permission denied\n", name);
    } else {
      exit_status = ERROR_EXIT;
      if (!options.s)
        fprintf(stderr, "grep: %s: No such file or directory\n", name);
    }
  } else {
    if (S_ISDIR(buf.st_mode)) {
      fclose(f);
      exit_status = ERROR_EXIT;
      if (!options.s) fprintf(stderr, "grep: %s: Is a directory\n", name);
    }
  }

  return exit_status;
}

void print_count(struct Options options, int match_count, char *name) {
  if (options.c) {
    if (!options.h) printf("%s:", name);
    if (options.l) {
      if (match_count > 0)
        printf("1\n");
      else
        printf("0");
    } else
      printf("%d\n", match_count);
  }
}

void print_names(struct Options options, int match_count, char *name) {
  if (options.l && match_count) printf("%s\n", name);
}
