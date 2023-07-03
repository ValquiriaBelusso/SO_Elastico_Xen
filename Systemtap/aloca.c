#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>

// gcc aloca.c -o aloca

int main(int argc, char *argv[])
{
      char command[100];
      size_t size = atoi(argv[1]);
      
	      long int free_mem;
	      long int total_mem;
	      long int mem_alocada;
	      long int mem;

	      free_mem = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576; // MB
	      total_mem = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576; //MB
	      mem_alocada = size + total_mem; // valor em megabytes
	      mem = total_mem * 0.2;

	      if (free_mem <= mem)
	      {
		    sprintf(command, "sudo xenstore-write -s /local/domain/12/memory/memalloc %ld \n", mem_alocada + 504); // lembrar de conferir o id
		    system(command);
		    fprintf(stderr, "%s", command);
	      }
}
