#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"
#include "stack.h"

int main(int argc, char **argv) {
  if (argc > 1) {
    int i;
    for( i = 0; i < argc; i++) {
      parse_file(argv[i]);
    }
  }
  else parse_file("stdin");
}
