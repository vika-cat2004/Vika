#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {  // проверка типа флагов и в соотв с ними изменять cat, исп как
                  // контейнер/синоним для др типа, не созд никак нов типы
                  // данных
  bool line_notempty;
  // bool symbol_end;
  bool line_number;  // флаг -n
  bool line_sgimempty;
  // bool show_tab;
} CatInfo;

void printFile(const char *fileName, const CatInfo *params);

int main(int argc, char *argv[]) {  // кол-во аргументов и указат на массив
                                    // указателей на строки
  CatInfo catParams = {
      .line_number = false, .line_notempty = false, .line_sgimempty = false};
  // Флаг для указания, нужно ли выводить номер строки
  // fileName = argv[1];
  // Проверка аргументов командной строки
  int startFileparam = -1;  // позиция названия ф-ла
  for (int j = 1; j < argc; j++) {
    // Установка флага, если обнаружена опция -n или --number
    if (strcmp(argv[j], "-n") == 0 ||
        strcmp(argv[j], "--number") == 0) {  // сравнение
      catParams.line_number = true;
      // printf("найден параметр %s,  параметр\n", argv[j]);
    } else if (strcmp(argv[j], "-b") == 0 ||
               strcmp(argv[j], "--number-nonblank") == 0) {
      catParams.line_notempty = true;
      // printf("найден параметр %s,  параметр\n", argv[j]);
    } else if (strcmp(argv[j], "-s") == 0 ||
               strcmp(argv[j], "--squeeze-blank") == 0) {
      catParams.line_sgimempty = true;
      // printf("найден параметр %s,  параметр\n", argv[j]);
    } else if (argv[j][0] == '-') {  // первый символ j-го аргумента
      printf("Ошибка, %s, неизвестный параметр\n", argv[j]);
      return 1;
    } else {
      (startFileparam = j);  // момент начала файлов
      printf("Название файла, %s\n", argv[j]);
      break;
    }
  }
  if (startFileparam == -1) {
    printf("Ошибка, нет файла для печати\n");
    return 1;
  }

  // printf("%s\n", argv[j]);
  // break;
  // fileName = argv[2];
  for (int j = startFileparam; j < argc; j++) {
    printFile(argv[j], &catParams);
  }
  return 0;
}
// файлнэйм имеет тип конст перемен *
void printFile(const char *fileName, const CatInfo *params) {
  int lineNumber = 1;                 // для нумерации строк
  FILE *file = fopen(fileName, "r");  // файловый дескриптор
  char buffer[256];  // куда будет считана строка
  int i = 0;         // номер текущего символа
  int previous_empty_lines = 0;

  if (!file) {  //== NULL
    printf("Файл: %s не может быть открыт\n", fileName);
    return;
  }

  // чтен стр, fgetc - чтен одного байта
  while ((buffer[i] = fgetc(file)) != EOF) {
    // сравнение, перенос строки Проверяем начало новой строки
    if (buffer[i] == '\n') {
      buffer[i] = '\0';  // присваивание, символ конца строки

      if (params->line_number) {
        printf("%6d\t%s\n", lineNumber, buffer);
        lineNumber++;
      } else if (params->line_notempty) {
        // длина строки
        if (strlen(buffer) != 0) {
          printf("%6d\t%s\n", lineNumber, buffer);
          lineNumber++;
        } else {
          printf("\n");
        }
      } else if (params->line_sgimempty) {
        if (strlen(buffer) != 0) {
          printf("%s\n", buffer);
          previous_empty_lines = 0;
        } else {
          if (!previous_empty_lines) {  // not null

            printf("\n");
            previous_empty_lines = 1;
          }
        }
      } else {  // нет парамтров в cat
        printf("%s\n", buffer);
      }
      i = 0;
      // lineNumber++;
    } else {
      i++;  // сдвиг на новый символ
    }
  }
  buffer[i] = '\0';  // печатает посл стр, символ конца строки
  if (params->line_number) {
    printf("%6d\t%s", lineNumber, buffer);
  } else if (params->line_notempty) {
    if (strlen(buffer) != 0) {
      printf("%6d\t%s", lineNumber, buffer);
    } else {
      printf("\n");
    }
  } else if (params->line_sgimempty) {
    if (strlen(buffer) != 0) {
      printf("%s", buffer);
      // previous_empty_lines = 0;
    } else {
      if (!previous_empty_lines) {  // not null
        printf("\n");
        // previous_empty_lines = 1;
      }
    }
  }

  else {  // нет парамтров в cat
    printf("%s", buffer);
  }

  fclose(file);
}