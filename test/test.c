#include <stdio.h>
#include <unistd.h>
#include "header.h"

int main() {
   int *x = get_var();

   while(1) {
      sleep(1);
      increment();
      printf("%d\n", *x);
   }

   return 0;
}
