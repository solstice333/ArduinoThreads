#include <stdio.h>

void func() {
}

int main() {
   int *ptr = malloc(5);

   printf("%p\n", func);
   printf("%x\n", func);

   int x = func;
   printf("%x\n", x);

   char *byte_ptr = &x;
   printf("%x\n", *byte_ptr++);
   printf("%x\n", *byte_ptr++);
   printf("%x\n", *byte_ptr++);


   return 0;
}
