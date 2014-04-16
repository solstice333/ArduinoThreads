#include <stdio.h>

void foo() {
   printf("bleh\n");
}

int main() {
   printf("address of foo: %d\n", foo);
   return 0;
}
