#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define SUCCESS_EXIT 0
#define ERROR_EXIT 1
#define STDIN_EXIT 2

struct flags_alphabet {
  char* flag;
  char* simple_flags;
};

void print_file(char* name, char* flags);
int checkfile(char* name, FILE* f);
void print_symbol(char c, char* prev, int* i, int* eline_printed, char* flags);
void print_stdin(char* flags);
int flags_parser(char* flags, int agrc, char** agrv, int* end_flags_index);
int append_flags(char* flags, char flag);
int append_GNU(char* flags, char* flag);
void append_flag(char* flags, char* new_flags);
void print_nonprint(char* c);

int main(int agrc, char** agrv) {
  char flags[7] = "\0";
  int end_flags_index = 1;
  int correct_input = flags_parser(flags, agrc, agrv, &end_flags_index);
  if (correct_input != ERROR_EXIT) {
    if (end_flags_index == agrc || correct_input == STDIN_EXIT) {
      print_stdin(flags);
    } else {
      for (int i = end_flags_index; i < agrc; i++) {
        print_file(agrv[i], flags);
      }
    }
  }
  return 0;
}

void print_file(char* name, char* flags) {
  FILE* f = NULL;
  f = fopen(name, "r");

  if (!checkfile(name, f)) {
    char c, prev = '\0';
    int i = 1, eline_printed = 0;
    c = getc(f);
    while (!feof(f)) {
      print_symbol(c, &prev, &i, &eline_printed, flags);
      c = getc(f);
    }
    fclose(f);
  }
}

int checkfile(char* name, FILE* f) {
  int exit_status = SUCCESS_EXIT;

  struct stat buf;
  stat(name, &buf);

  if (f == NULL) {
    if (S_ISREG(buf.st_mode)) {
      exit_status = ERROR_EXIT;
      fprintf(stderr, "cat: %s: Permission denied\n", name);
    } else {
      exit_status = ERROR_EXIT;
      fprintf(stderr, "cat: %s: No such file or directory\n", name);
    }
  } else {
    if (S_ISDIR(buf.st_mode)) {
      fclose(f);
      exit_status = ERROR_EXIT;
      fprintf(stderr, "cat: %s: Is a directory\n", name);
    }
  }

  return exit_status;
}

void print_symbol(char c, char* prev, int* i, int* eline_printed, char* flags) {
  if (!(strchr(flags, 's') != NULL && *prev == '\n' && c == '\n' &&
        *eline_printed)) {
    if (*prev == '\n' && c == '\n')
      *eline_printed = 1;
    else
      *eline_printed = 0;

    if (strchr(flags, 'b') != NULL) {
      if (*i == 1 && c != '\n') {
        printf("%6d\t", *i);
        (*i)++;
      }
      if (c != '\n' && *prev == '\n') {
        printf("%6d\t", *i);
        (*i)++;
      }
      if (strchr(flags, 'v') != NULL && c != '\n' && (signed char)c != -118 &&
          (signed char)*prev == -118) {
        printf("%6d\t", *i);
        (*i)++;
      }
    }

    if (strchr(flags, 'n') != NULL) {
      if (*i == 1) {
        printf("%6d\t", *i);
        (*i)++;
      }
      if (*prev == '\n') {
        printf("%6d\t", *i);
        (*i)++;
      }
      if (strchr(flags, 'v') != NULL && c != '\n' && (signed char)c != -118 &&
          (signed char)*prev == -118) {
        printf("%6d\t", *i);
        (*i)++;
      }
    }
    *prev = c;
    if (strchr(flags, 'v') != NULL && c != '\n' && c != '\t')
      print_nonprint(&c);
    if (strchr(flags, 'E') != NULL && c == '\n') printf("$");
    if (strchr(flags, 'T') != NULL && c == '\t') {
      printf("^");
      c = 'I';
    }

    printf("%c", c);
  }
}

void print_stdin(char* flags) {
  char c, prev = '\0';
  int i = 1, eline_printed = 0;
  while ((c = fgetc(stdin)) != EOF) {
    print_symbol(c, &prev, &i, &eline_printed, flags);
  }
}

int flags_parser(char* flags, int agrc, char** agrv, int* end_flags_index) {
  int exit_status = SUCCESS_EXIT;
  for (int i = 1; i < agrc; i++) {
    if (!strcmp(agrv[i], "-")) {
      exit_status = STDIN_EXIT;
      i = agrc;
    } else if (!strcmp(agrv[i], "--")) {
      (*end_flags_index)++;
      i = agrc;
    } else if (agrv[i][0] != '-') {
      i = agrc;
    } else {
      (*end_flags_index)++;
      if (agrv[i][1] == '-')
        exit_status = append_GNU(flags, agrv[i]);
      else {
        for (size_t j = 1; j < strlen(agrv[i]); j++) {
          exit_status = append_flags(flags, agrv[i][j]);
          if (exit_status == ERROR_EXIT) j = strlen(agrv[i]);
        }
      }
    }
    if (exit_status == ERROR_EXIT) i = agrc;
  }

  return exit_status;
}

int append_flags(char* flags, char flag) {
  int exit_status = SUCCESS_EXIT, count = 0;
  ;
  struct flags_alphabet flags_alph[8] = {{"b", "b"},  {"E", "E"}, {"e", "Ev"},
                                         {"n", "n"},  {"s", "s"}, {"T", "T"},
                                         {"t", "Tv"}, {"v", "v"}};
  for (int i = 0; i < 8; i++) {
    if (flag == flags_alph[i].flag[0])
      append_flag(flags, flags_alph[i].simple_flags);
    else
      count++;
  }
  if (count == 8) {
    fprintf(stderr, "cat: Illegal option -- %c\n", flag);
    exit_status = ERROR_EXIT;
  }

  return exit_status;
}

int append_GNU(char* flags, char* flag) {
  int exit_status = SUCCESS_EXIT, count = 0;
  struct flags_alphabet flags_alph[3] = {
      {"--number-nonblank", "b"}, {"--number", "n"}, {"--squeeze-blank", "s"}};
  for (int i = 0; i < 3; i++) {
    if (!strcmp(flag, flags_alph[i].flag))
      append_flag(flags, flags_alph[i].simple_flags);
    else
      count++;
  }
  if (count == 3) {
    fprintf(stderr, "cat: Illegal option %s\n", flag);
    exit_status = ERROR_EXIT;
  }

  return exit_status;
}

void append_flag(char* flags, char* new_flags) {
  for (size_t i = 0; i < strlen(new_flags); i++) {
    if (strchr(flags, new_flags[i]) == NULL &&
        !(new_flags[i] == 'n' && strchr(flags, 'b') != NULL)) {
      if (new_flags[i] == 'b') {
        int j = 0;
        while (flags[j] != 'n' && flags[j] != '\0') j++;
        flags[j] = new_flags[i];
      } else {
        char str[2];
        str[0] = new_flags[i];
        str[1] = '\0';
        strcat(flags, str);
      }
    }
  }
}

void print_nonprint(char* c) {
  if (*c >= 0 && *c <= 31) {
    printf("^");
    *c += 64;
  }
  if (*c == 127) {
    printf("^");
    *c -= 64;
  }
  if ((signed char)*c <= -97) {
    printf("M-");
    *c += 128;
    print_nonprint(c);
  }
}