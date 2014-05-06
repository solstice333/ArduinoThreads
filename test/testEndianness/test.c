#include <stdio.h>

int main() {
   int x = 0x45A1BC23;
   char *c_ptr = &x;

   int i;
   printf("x: %x\n", x);
   printf("traversing x: \n");
   for (i = 0; i < 4; i++) {
      printf("%x\n", *c_ptr++);
   }

   return 0;
}
