#include "header.h"

static int x = 0;

void increment() {
   ++x;
}

int *get_var() {
   return &x;
}
