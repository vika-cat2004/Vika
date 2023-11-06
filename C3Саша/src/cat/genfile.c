#include <stdio.h>

int main() {
  FILE *f = fopen("test_file", "w+");

  for (int i = 0; i < 256; i++) {
    putc(i, f);
    if (i % 25 == 0) {
      putc('\n', f);
      if (i % 75 == 0) {
        putc('\n', f);
        putc('\n', f);
        putc('\n', f);
      }
    }
  }

  fclose(f);
  return 0;
}