#include "s21_grep.h"

int main(int argc, char *argv[]) {
  Flags flags = {0};
  set_flags(argc, argv, &flags);
  open_file(argc, argv, &flags);
  return 0;
}

void set_flags(int argc, char *argv[], Flags *flags) {
  int option = 0;
  while ((option = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (option) {
      case 'e':
        flags->e = 1;
        strcat(flags->regulars, optarg);
        strcat(flags->regulars, "|");
        break;
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'f':
        flags->f = 1;
        flag_f(flags);
        break;
      case 'o':
        flags->o = 1;
        break;
      default:
        fprintf(stderr, "Usage grep [flags] [template] [file ...]");
    }
  }

  if (!flags->e && !flags->f) {
    if (argc > optind) {
      strcat(flags->regulars, argv[optind]);
    }
    optind++;
  }
  if (flags->e) {
    flags->regulars[strlen(flags->regulars) - 1] = '\0';
  }
  if (flags->v && flags->o) {  //
    flags->o = 0;
  }
}

void open_file(int argc, char *argv[], Flags *flags) {
  int count_files = argc - optind;
  while (optind < argc) {
    FILE *filename = fopen(argv[optind], "r");
    if (filename != NULL) {
      processing(count_files, argv, filename, flags);
      optind++;
      fclose(filename);
    } else {
      if (!flags->s) {
        fprintf(stderr, "grep: %s: No such file or directory\n", argv[optind]);
      }
      optind++;
    }
  }
}

void processing(int count_files, char *argv[], FILE *cur_file, Flags *flags) {
  regex_t reg;
  regmatch_t match;
  int found = 0;
  char buffer[4096];
  int flag_i;
  int count_lines = 0, count_matches = 0;
  if (flags->i) {
    flag_i = REG_EXTENDED | REG_ICASE;
  } else {
    flag_i = REG_EXTENDED;
  }
  regcomp(&reg, flags->regulars, flag_i);
  while (!feof(cur_file)) {
    count_lines++;
    fgets(buffer, sizeof(buffer), cur_file);
    int check = regexec(&reg, buffer, 1, &match, 0);
    if (flags->v) {
      check = !check;
    }
    if (check == 0) {
      count_matches++;
      found = 1;
      if (!flags->c && !flags->l) {
        precede_output(flags, argv[optind], count_lines, count_files);
        output_result(buffer, flags, reg);
        if (buffer[strlen(buffer) - 1] != '\n') {
          fputs("\n", stdout);
        }
      }
    }
  }
  if (flags->c) {
    precede_output(flags, argv[optind], count_lines, count_files);
    if (flags->l && found) {
      fputc('1', stdout);
      fputc('\n', stdout);
    } else {
      fprintf(stdout, "%d\n", count_matches);
    }
  }
  if (flags->l && found) {
    fputs(argv[optind], stdout);
    fputc('\n', stdout);
  }
  regfree(&reg);
}

void precede_output(Flags *flags, char *filename, int count_lines,
                    int files_num) {
  if (!flags->h && files_num > 1) {
    fprintf(stdout, "%s:", filename);
  }
  if (flags->n && !flags->c) {
    fprintf(stdout, "%d:", count_lines);
  }
}

void output_result(char *buffer, Flags *flags, regex_t reg) {
  if (flags->o) {
    output_substring(buffer, reg);
  } else {
    fputs(buffer, stdout);
  }
}

void flag_f(Flags *flags) {
  char filename[4096];
  strcpy(filename, optarg);
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "grep: %s: %s", filename, "No such file or directory\n");
  } else {
    char string[4096];
    fgets(string, sizeof(string), file);
    strcat(flags->regulars, string);
    fclose(file);
  }
}

void output_substring(char *buffer, regex_t reg) {
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