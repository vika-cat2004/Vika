// #include  <errno.h>
#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

// #define BUFFER 4096

void open_file(FILE *file) {
  int name = 0;
  while ((name = getc(file)) != EOF) {
    putc(name, stdout);
  }
}
// указат на переменную
CatInfo read_flags(int argc, char *argv[], int *invalid) {
  struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                  {"number", 0, NULL, 'n'},
                                  {"squeeze-blank", 0, NULL, 's'},
                                  {NULL, 0, NULL, 0}};
  int parse_params = getopt_long(argc, argv, "beEnstTv", long_options, NULL);
  CatInfo flag = {0};
  while (parse_params != -1) {
    switch (parse_params) {
      case 'b':
        flag.b = 1;
        break;
      case 'e':
        flag.e = 1;
        flag.v = 1;
        break;
      case 'E':
        flag.e = 1;
        break;
      case 'n':
        flag.n = 1;
        break;
      case 's':
        flag.s = 1;
        break;
      case 't':
        flag.t = 1;
        flag.v = 1;
        break;
      case 'T':
        flag.t = 1;
        break;
      case 'v':
        flag.v = 1;
        break;
      case '?':
        *invalid = 1;
        break;
    }
    parse_params = getopt_long(argc, argv, "beEnstTv", long_options, NULL);
  }
  return flag;
}

// -b -n
void number(FILE *file, CatInfo flag) {
  int line_number;
  int count = 0;
  int last = '\n';
  while ((line_number = getc(file)) != EOF) {
    if (last == '\n') {
      if (flag.b) {
        if (line_number != '\n') printf("%6i\t", ++count);
      } else if (flag.n) {
        printf("%6i\t", ++count);
      }
    }
    putc(line_number, stdout);
    last = line_number;
  }
}

void symbol_e(FILE *file) {
  int c;
  while ((c = getc(file)) != EOF) {
    if (c == 10) printf("$");

    if (c >= 0 && c < 32 && c != 9 && c != 10) {
      printf("^");
      c += 64;
    } else if (c == 127) {
      printf("^");
      c = 63;  // код вопросительного знака
    } else if (c >= 128 && c < 160) {
      printf("M-^");
      c -= 64;
    } else if (c >= 160 && c != 255) {
      printf("M-");
      c -= 128;
    } else if (c == 255)
      printf("M-^?");

    putc(c, stdout);
  }
}
//-E
void symbol_E(FILE *file) {
  int c;
  while ((c = getc(file)) != EOF) {
    if (c == '\n') {
      printf("$");
    }
    putc(c, stdout);
  }
}

//-v
void visible(FILE *file) {
  int c;
  while ((c = getc(file)) != EOF) {
    if ((c < 32 && c != '\n' && c != '\t') || c == 127) printf("^");
    if (c > 127 && c < 160) printf("M-^");
    if (c > 159 && c < 255) {
      printf("M-");
      c = c + 128;
    } else if ((c < 32 || (c > 126 && c < 255)) && c != '\n' && c != '\t') {
      if (c > 126 && c < 160)
        c = c - 64;
      else if (c < 126)
        c = c + 64;
    }

    if (c == 255)
      printf("M-^?");
    else if (c == 9) {
      printf("^I");
    } else
      putc(c, stdout);
  }
}

//-T
void tab_T(FILE *file) {
  int c;
  while ((c = getc(file)) != EOF) {
    if (c != '\t')
      putc(c, stdout);
    else
      printf("^I");
  }
}

//-s
void sgimempty(FILE *file) {
  int line = 0;
  int last = '\n';
  int s_line = 0;
  while ((line = getc(file)) != EOF) {
    if (line == '\n' && last == '\n') {
      if (s_line) continue;
      s_line = 1;
    } else
      s_line = 0;
    putc(line, stdout);
    last = line;
  }
}

void cat(FILE *file, CatInfo flag) {
  if (flag.b || flag.n)  // или
    number(file, flag);
  else if (flag.e && flag.v)  // и
    symbol_e(file);
  else if (flag.e)
    symbol_E(file);
  else if (flag.s)
    sgimempty(file);
  else if (flag.t && flag.v)
    visible(file);  /////////////////////////////////// tab_t
  else if (flag.t)
    tab_T(file);
  else if (flag.v)
    visible(file);
  else  // open with no flag
    open_file(file);
}

void file(int argc, char *argv[], CatInfo flag) {
  // созд указат на 2й элт, провер, что указат не достиг конца списка арг
  for (char **filename = &argv[1], **end = &argv[argc]; filename != end;
       ++filename) {
    if (**filename == '-') continue;
    // rb - чтение текстовых и бинарных флов
    FILE *doc = fopen(*filename, "rb");
    if (doc == NULL) {  // invalid option + open file
      printf("cat: %s: No such file\n", *filename);
    } else {
      cat(doc, flag);
      fclose(doc);
    }
  }
}

int main(int argc, char *argv[]) {
  int invalid = 0;
  CatInfo flag = read_flags(argc, argv, &invalid);
  if (invalid == 0) {
    file(argc, argv, flag);
  }

  return 0;
}
