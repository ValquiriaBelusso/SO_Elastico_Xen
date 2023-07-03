#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>

// gcc -fPIC -shared -o libmalloc.so libmalloc4.c -ldl
// LD_PRELOAD=./libmalloc.so ./str

static void *(*real_malloc)(size_t) = NULL;
static void *(*real_realloc)(void *, size_t) = NULL;
static void *(*real_calloc)(size_t, size_t) = NULL;


static int alloc_init_pending = 0;

/* Load original allocation routines at first use */
static void alloc_init(void)
{
      alloc_init_pending = 1;
      real_malloc = dlsym(RTLD_NEXT, "malloc");
      real_realloc = dlsym(RTLD_NEXT, "realloc");
      real_calloc = dlsym(RTLD_NEXT, "calloc");
      if (!real_malloc || !real_realloc || !real_calloc)
      {
            fputs("libmalloc.so: Unable to hook allocation!\n", stderr);
            fputs(dlerror(), stderr);
            exit(1);
      }
      // else {
      // fputs("libmalloc.so: Successfully hooked\n", stderr);
      //}
      alloc_init_pending = 0;
}

#define ZALLOC_MAX 1024
static void *zalloc_list[ZALLOC_MAX];
static size_t zalloc_cnt = 0;

void *zalloc_internal(size_t size)
{
      fputs("alloc.so: zalloc_internal called", stderr);
      if (zalloc_cnt >= ZALLOC_MAX - 1)
      {
            fputs("alloc.so: Out of internal memory\n", stderr);
            return NULL;
      }
      /* Anonymous mapping ensures that pages are zero'd */
      void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
      if (MAP_FAILED == ptr)
      {
            perror("alloc.so: zalloc_internal mmap failed");
            return NULL;
      }
      zalloc_list[zalloc_cnt++] = ptr; /* keep track for later calls to free */
      return ptr;
}


void *malloc(size_t size)
{
      char command[100];
      if (size > 102400)
      {

            if (alloc_init_pending)
            {
                  fputs("alloc.so: malloc internal\n", stderr);
                  return zalloc_internal(size);
            }

            if (!real_malloc)
            {
                  alloc_init();
            }

            void *result;
            
            long int total_mem = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576; //KB
            long int mem_alocada = size/1024 + total_mem; // valor está em megabytes
            long int mem = total_mem * 0.2;

            if (real_malloc == NULL)
            {
                  alloc_init();
            }

	    long int free_mem = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576;
	    
	    float percentual;
	    
	    percentual=free_mem/total_mem;
	    
            if (percentual <= 0.2)
            {
                  sprintf(command, "sudo xenstore-write -s /local/domain/1/memory/memalloc %ld", (mem_alocada) + 300); 
                  system(command);
                  fprintf(stderr, "Command:  %s\n", command);
            }
            result = real_malloc(size);
            return result;
      }
      else
      {
            if (!real_malloc)
            {
                  alloc_init();
            }
            void *result = real_malloc(size);
            return result;
      }
}

void *realloc(void *ptr, size_t size)
{

      char command[100];

      if (size > 102400)
      {

             if (alloc_init_pending)
            {
                  fputs("alloc.so: realloc internal\n", stderr);
                  return zalloc_internal(size);
            }

            if (!real_realloc)
            {
                  alloc_init();
            }

            void *result;
            
            long int total_mem = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576;
            long int mem_alocada = size/1024 + total_mem; // valor está em megabytes
            long int mem = total_mem * 0.2;

            if (real_malloc == NULL)
            {
                  alloc_init();
            }

            long int free_mem = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576;
	    
	    float percentual;
	    
	    percentual=free_mem/total_mem;
	    
            if (percentual <= 0.2)
            {
                  sprintf(command, "sudo xenstore-write -s /local/domain/1/memory/memalloc %ld", mem_alocada + 300); // alterar para o dom adequado
                  system(command);
                  fprintf(stderr, "Command: %s\n", command);
            }
      }

      void *result = real_realloc(ptr,size);
      return result;
}

void *calloc(size_t nmemb, size_t size)
{

      size_t newsize;
      newsize = nmemb * size;

      char command[100];


      if (newsize > 102400)
      {
            if (alloc_init_pending)
            {
                  fputs("alloc.so: realloc internal\n", stderr);
                  return zalloc_internal(size);
            }

            if (!real_calloc)
            {
                  alloc_init();
            }

            void *result;
            
            long int total_mem = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576; //MB
            long int mem_alocada = newsize/1024 + total_mem; // valor está em megabytes
            long int mem = total_mem * 0.2;

            if (real_malloc == NULL)
            {
                  alloc_init();
            }

            long int free_mem = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) / 1048576; //MB
	    
	    float percentual;
	    
	    percentual=free_mem/total_mem;
	    
            if (percentual <= 0.2)
            {
                  sprintf(command, "sudo xenstore-write -s /local/domain/1/memory/memalloc %ld", (mem_alocada) + 300); // alterar para o dom adequado
                  system(command);
                  fprintf(stderr, "Command:  %s\n", command);
                  fflush(stdout);
                  fprintf(stderr, "fim\n");
            }

      }
      
      void *result = real_calloc(nmemb, size);
      return result;
}

