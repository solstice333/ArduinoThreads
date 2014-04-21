#include <stdio.h>

typedef enum {
   false = 0, true = 1
} boolean;

int main() {
   printf("false: : %d\n", false);
   printf("true: : %d\n", true);

   boolean x;
   x = true;
   printf("x when true: %d\n", x);
   x = false;
   printf("x when false: %d\n", x);
   x = true;

   if (x) {
      printf("This is true");
   }

   return 0;
}

