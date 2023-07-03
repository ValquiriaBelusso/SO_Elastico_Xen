#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define tempo 30 // testar com tempo de 10 e 30
#define MEGA 1024*1024

// gcc -o teste2 teste2.c
// ./teste2

int *ptr;

int main()
{
      srand(1);

      long int x = 50 * MEGA;
      long int y = 500 * MEGA;

      printf(" 0: %ld \n", x);
      fflush(stdout);
      ptr = malloc(x);
      memset(ptr,1,x);
      sleep(tempo);
      //free(ptr);

      int time = 10;
      for (int i = 1; i < 6; i++)
      {
            x = x + (rand() % y);

            printf(" %d: %ld \n", i, x);
            fflush(stdout);
            ptr = realloc(ptr,x);
            memset(ptr,1,x);
            sleep(tempo);
            time = time + tempo;
            //free(ptr);
      }
}
