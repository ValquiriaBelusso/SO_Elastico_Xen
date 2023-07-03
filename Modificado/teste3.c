#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define tempo 30 // testar com tempo de 10 e 30c
//#define MEGA 1024*1024

// gcc -o teste3 teste3.c
// ./teste2

int *ptr;

int main()
{    
      //sleep(tempo);
      unsigned long int y = 500000000;

      printf(" 0: %lu \n", y);
      fflush(stdout);
      ptr = calloc(3,y);
      memset(ptr,1,y);
      free(ptr);
      sleep(tempo);
}
      
