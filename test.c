#include <stdio.h>

#define numToAsciiHex(type, num, container) {\
   type __copy = (num), __quo = (num), __rem;\
   int __i, __length;\
   \
   for (__length = 0; __copy; __length++)\
      __copy /= 16;\
   \
   for (__i = __length - 1; __i >= 0; __i--) {\
      __rem = (__quo) % 16;\
   \
      switch (__rem) {\
         case 10:\
            __rem = 'A';\
            break;\
         case 11:\
            __rem = 'B';\
            break;\
         case 12:\
            __rem = 'C';\
            break;\
         case 13:\
            __rem = 'D';\
            break;\
         case 14:\
            __rem = 'E';\
            break;\
         case 15:\
            __rem = 'F';\
            break;\
         default:\
            __rem += '0';\
      }\
      (container)[__i] = __rem;\
      (__quo) /= 16;\
   }\
   (container)[__length] = 0;\
}\

int main() {
   int arr[20], *runner = arr;

   numToAsciiHex(int, 65120, arr);
   while (*runner)
      printf("%c ", *runner++);
   printf("\n");

   return 0;
}
