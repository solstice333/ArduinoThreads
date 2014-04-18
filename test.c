#include <stdio.h>

typedef struct thread {
   int active;
} thread;


int main() {
   int i = 0;
   int current = 1;
   int save = current;
   printf("save: %d\n", current);
   thread threadlist[8];

   for (i = 0; i < 8; i++)
      threadlist[i].active = i == 4 ? 0 : 0;

   if (current >= 8)
      current = 7;

   for (i = (current + 1) % 8; i != current && !threadlist[i].active;
    i = (i + 1) % 8) 
      printf("%d\n", i);

   if (i == current && !threadlist[i].active) {
      current = save;
      i = 255;
   }

   printf("returning: %d\n", i);

   return 0;
}
