#include <stdio.h>
#include <xenstore.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// para compilar
// gcc xen_mem.c -o out -lxenstore
// sudo ./out

// sudo xl create xen2.cfg
// vncviewer localhost:5900

int main()
{
    struct xs_handle *xs;
    xs_transaction_t th;
    char *path;
    int fd;
    fd_set set;
    int er;
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 0};

    char **vec;
    unsigned int num_strings;
    char *buf;
    unsigned int len;

    /* Get a connection to the daemon */
    xs = xs_open(0);

    if (xs == NULL)
    {
        printf("erro conexao xs\n");
        exit(1);
    } 

    // /* Get the local domain path */
    path = xs_get_domain_path(xs, 4); // alterar para o dom adequado

    if (path == NULL)
    {
        printf("erro get domain path\n");
        exit(1);
    }

    /* Make space for our node on the path */
    path = realloc(path, strlen(path) + strlen("/memory") + 1);
    if (path == NULL)
    {
        printf("erro alocacao path\n");
        exit(1);
    }

    strcat(path, "/memory");

    /* Create a watch on /local/domain/%d/mynode. */
    er = xs_watch(xs, path, "memory");

    if (er == 0)
    {
        printf("erro watch\n");
        exit(1);
    }

    /* We are notified of read availability on the watch via the file descriptor. */
    fd = xs_fileno(xs);

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);

        /* Poll for data. */
        if (select(fd + 1, &set, NULL, NULL, &tv) > 0 && FD_ISSET(fd, &set))
        {
            /* num_strings will be set to the number of elements in vec* (typically, 2 - the watched path and the token) */
            vec = xs_read_watch(xs, &num_strings);
            if (!vec)
            {
                printf("erro leitura vetor\n");
                exit(1);
            }

            printf("vec contents: %s | %s\n", vec[XS_WATCH_PATH], vec[XS_WATCH_TOKEN]);

            /* Prepare a transaction and do a read. */
            th = xs_transaction_start(xs);
            buf = xs_read(xs, th, vec[XS_WATCH_PATH], &len);
            xs_transaction_end(xs, th, false);
        }

        int nbuf;
        int value = atoi(buf);
        nbuf = value;
        
        //printf("%d",nbuf);

        if ((value != 0)&&(!strcmp(vec[XS_WATCH_PATH], "/local/domain/4/memory/memalloc"))) // alterar de 1 para o dom adequado
        { // valor em bytes da memoria
            fflush(stdout);
            char command[200];
            sprintf(command, "sudo xl mem-set Xen3 %d", nbuf); //conferir se o xen está corrreto
            printf("[MEMORY REQUEST:] %d\n", nbuf);fflush(stdout);  
            system(command);
        }
        //sleep(3);
    }
}
