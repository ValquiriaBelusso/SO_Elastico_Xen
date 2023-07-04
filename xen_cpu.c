#include <stdio.h>
#include <xenstore.h> 
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// para compilar
// gcc xen_cpu.c -o cpu -lxenstore
// sudo ./cpu

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
    char *token;
    const char s[2] = "/";
    int flag;

    /* Get a connection to the daemon */
    xs = xs_open(0);

    if (xs == NULL)
    {
        printf("erro conexao xs\n");
        exit(1);
    }

    // /* Get the local domain path */
    path = xs_get_domain_path(xs, 7); // alterar de 1 para o dom adequado

    if (path == NULL)
    {
        printf("erro get domain path\n");
        exit(1);
    }

    /* Make space for our node on the path */
    path = realloc(path, strlen(path) + strlen("/cpu") + strlen("/") + 5 + strlen("/availability"));
    if (path == NULL)
    {
        printf("erro alocacao path\n");
        exit(1);
    }
    // else
    // printf("PATH: %s\n", path);
    //}

    /* Create a watch on /local/domain/%d/mynode. */
    er = xs_watch(xs, path, "cpu");

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
        flag = 0;

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

            /* Prepare a transaction and do a read. */
            th = xs_transaction_start(xs);
            buf = xs_read(xs, th, vec[XS_WATCH_PATH], &len);
            xs_transaction_end(xs, th, false);
            token = strtok(vec[XS_WATCH_PATH], s);

            for (int i = 0; i < 4; i++)
            {
                token = strtok(NULL, s);
            }

            if (!strncmp(buf, "online", 6) && (token != NULL))
            {

                int value = atoi(token);
                printf("[REQUEST CPU:] %d \n", value); // aqui está o numero do processador que foi requisitado

                if (value != 0)
                {
                    char command[100];
                    sprintf(command, "sudo xl vcpu-set Xen2 %d", value);
                    system(command);
                    flag = 1;
                    printf("[CPU-SET:] %d \n", value); // aqui está o numero do processador que foi ligado (ou desligado)
                }
            }

            if (flag == 1)
            {
                for (int k = 0; k < 8; k++)
                {
                    vec = xs_read_watch(xs, &num_strings);
                    if (!vec)
                    {
                        printf("erro leitura vetor\n");
                        exit(1);
                    }

                    th = xs_transaction_start(xs);
                    buf = xs_read(xs, th, vec[XS_WATCH_PATH], &len);
                    xs_transaction_end(xs, th, false);
                }
                flag = 0;
            }
        }
    }
}
