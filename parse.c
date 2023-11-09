#include "parse.h"

#include <getopt.h>

int parse_params(int argc, char **argv, char *short_options,
                 const struct option *long_options, config *conf) {
  int res;  // записываем результат
  int error = 0;
  int idx = 0;  // куда будет сохран индекс найд опции, текущ параметра, в
                // массиве long_options

  while ((res = getopt_long(argc, argv, short_options, long_options, &idx)) !=
         -1) {
    // //getopt_long считывает все параметры по очереди и сравнивает их с теми,
    // что мы передаем в качестве ожидаемых.
    // - Если она встречает ожидаемый флаг, то она возвращает его int
    // представление
    // - Если встречается не ожидаемый флаг, то возвращается символ ‘?’
    // - Если параметры закончились, то возвращается -1

    switch (res) {
      case 'b':
        conf->b = 1;
        break;
      case 'e':
        conf->e = 1;
        conf->v = 1;
        break;
      case 'E':
        conf->e = 1;
        break;
      case 'n':
        conf->n = 1;
        break;
      case 's':
        conf->s = 1;
        break;
      case 't':
        conf->t = 1;
        conf->v = 1;
        break;
      case 'T':
        conf->t = 1;
      case 'v':
        conf->v = 1;
        break;
      case '?':
        error = 1;
        break;
      default:
        error = 1;
        break;
    }
  }

  if (error) {
    return 1;
  }
  return 0;
}

// сравн наш res со всевозмож вариантами ожидаемых флагов и в тоже время сохр
// состояние наших флагов в нашу структуру сonf, которая пришла к нам в функцию
// по ссылке из параметров самой функции.

// 📌 Заметь, если не знал. Если структура пришла к нам по ссылке, то обращение
// к ее внутренним полям происходит через “->”, а не как обычно, через
// “.”(точку)