#include <stdio.h>

int main() {
  FILE *f = fopen("test_file", "w+");
  fprintf(f,
          "Looking for pattern\nDont print it\nPrint this "
          "(pattern)\nPaTtErN\nlookpatternhere\npatternpatternpattern\n;"
          "adsfjka;lsdf");
  fclose(f);

  f = fopen("pattern_file", "w+");
  fprintf(f, "pattern\nlook");
  fclose(f);

  return 0;
}