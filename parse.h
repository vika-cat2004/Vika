#ifndef PARSE_H
#define PARSE_H
#include <getopt.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} config;

int parse_params(int argc, char **argv, char *short_options,
                 const struct option *long_options, config *conf);

#endif