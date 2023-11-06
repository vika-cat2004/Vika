#include "s21_grep.h"

struct Options grepFlag = {.e = 0,
                           .i = 0,
                           .v = 0,
                           .c = 0,
                           .l = 0,
                           .n = 0,
                           .h = 0,
                           .f = 0,
                           .o = 0,
                           .err = 0};

int main(int argc, char **argv) {
  parseFlags(argc, argv, &grepFlag);
  processFiles(argc, argv, grepFlag);
  return 0;
}

void parseFlags(int argc, char **argv, Options *grepFlag) {
  int rez = 0;
  while ((rez = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (rez) {
      case 'e':
        grepFlag->e = 1;
        strcat(grepFlag->reg_exp, optarg);
        strcat(grepFlag->reg_exp, "|");
        break;
      case 'i':
        grepFlag->i = 1;
        break;
      case 'v':
        grepFlag->v = 1;
        break;
      case 'c':
        grepFlag->c = 1;
        break;
      case 'l':
        grepFlag->l = 1;
        break;
      case 'n':
        grepFlag->n = 1;
        break;
      case 'h':
        grepFlag->h = 1;
        break;
      case 's':
        grepFlag->s = 1;
        break;
      case 'f':
        grepFlag->f = 1;
        readRegex(grepFlag);
        break;
      case 'o':
        grepFlag->o = 1;
        break;
      case '?':
        grepFlag->err = 1;
        break;
    }
  }
  if (grepFlag->e == 0 && grepFlag->f == 0) {
    if (argc > optind) {
      strcat(grepFlag->reg_exp, argv[optind]);
    }
    optind++;
  }
  if (grepFlag->e == 1) {
    grepFlag->reg_exp[strlen(grepFlag->reg_exp) - 1] = '\0';
  }
  if (grepFlag->v == 1 && grepFlag->o == 1) {
    grepFlag->o = 0;
  }
}

void readRegex(Options *grepFlag) {
  char fileName[256];
  char string[256];
  strcpy(fileName, optarg);
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    fprintf(stderr, "grep: %s: %s", fileName, "No such file or directory\n");
  } else {
    fgets(string, sizeof(string), file);
    strcat(grepFlag->reg_exp, string);
  }
  fclose(file);
}

void processFiles(int argc, char **argv, Options grepFlag) {
  if (grepFlag.err != 1) {
    int filesNumber = argc - optind;
    while (optind < argc) {
      FILE *file = fopen(argv[optind], "r");
      if (file != NULL) {
        parseFile(filesNumber, argv, file, grepFlag);
        optind++;
      } else {
        if (grepFlag.s == 0) {
          fprintf(stderr, "grep: %s: %s", argv[optind],
                  "No such file or directory\n");
        }
        optind++;
      }
      fclose(file);
    }
  } else {
    fputs("invalid option -- ", stderr);
  }
}

void parseFile(int filesNumber, char **argv, FILE *file, Options grepFlag) {
  regex_t reg;
  regmatch_t match;
  int result;
  int outputFile = 0;
  char line[256];
  int c_flag = REG_EXTENDED;
  int countLines = 0, countMatchings = 0;
  if (grepFlag.i == 1) {
    c_flag |= REG_ICASE;
  }
  regcomp(&reg, grepFlag.reg_exp, c_flag);
  while (fgets(line, sizeof(line), file) != NULL) {
    countLines++;
    result = regexec(&reg, line, 1, &match, 0);
    if (grepFlag.v == 1) {
      result = !result;
    }
    if (result == 0) {
      countMatchings++;
      outputFile = 1;
      if (grepFlag.c == 0 && grepFlag.l == 0) {
        fileAndNumberOutput(grepFlag, argv[optind], countLines, filesNumber);
        outputResult(line, grepFlag, reg);
        if (line[strlen(line) - 1] != '\n') {
          fputs("\n", stdout);
        }
      }
    }
  }
  if (grepFlag.c == 1) {
    fileAndNumberOutput(grepFlag, argv[optind], countLines, filesNumber);
    if (grepFlag.l == 1 && outputFile == 1) {
      fputc('1', stdout);
      fputc('\n', stdout);
    } else {
      fprintf(stdout, "%d\n", countMatchings);
    }
  }
  if (grepFlag.l == 1 && outputFile == 1) {
    fputs(argv[optind], stdout);
    fputc('\n', stdout);
  }
  regfree(&reg);
}

void fileAndNumberOutput(Options grepFlag, char *filename, int count_lines,
                         int files_num) {
  if (grepFlag.h == 0 && files_num > 1) {
    fprintf(stdout, "%s:", filename);
  }
  if (grepFlag.n == 1 && grepFlag.c == 0) {
    fprintf(stdout, "%d:", count_lines);
  }
}

void outputResult(char *buffer, Options grepFlag, regex_t reg) {
  if (grepFlag.o == 1) {
    outputFlagO(buffer, reg);
  } else {
    fputs(buffer, stdout);
  }
}

void outputFlagO(char *buffer, regex_t reg) {
  regmatch_t matches[1];
  while (regexec(&reg, buffer, 1, matches, 0) == 0) {
    printf("%.*s", (int)(matches[0].rm_eo - matches[0].rm_so),
           buffer + matches[0].rm_so);
    buffer += matches[0].rm_eo;
    if (*buffer != '\0') {
      fputc('\n', stdout);
    }
  }
}
