#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "os.h"

int main() {
   serial_init();

   char *x = malloc(32);
   strcpy(x, "Kevin Navero");

   print_string(x);
   return 0;
}
