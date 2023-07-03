#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

//como executar
//gcc -o teste2 teste2_2t.c -fopenmp -lm
//./teste2 10000000000

int primo (long int n)
{
    long int i ;
    for (i = 3; i < (int)(sqrt ( n )+ 1); i+=2)
      if ( n % i == 0)
         return 0;
         
    return 1;
}

int main ( int argc , char * argv [])
{ 
    int total=0;
    long int i, n;
    long int x;

    if ( argc < 2)
    {
        printf("Valor invalido! Entre com o valor do maior inteiro \n") ;
        return 0;
    }
    else
    {
        n = strtol(argv[1], (char **) NULL , 10) ;
    }
    
    #pragma omp parallel private (x) num_threads(2)
    {
        double ini = omp_get_wtime();
        x=0;
        #pragma omp for reduction(+:total) schedule(dynamic, 100) nowait
        for ( i = 3; i <= n ; i += 2)
        {
            x++;
            if ( primo ( i ) == 1)
                total ++;
        }
        double fim = omp_get_wtime();
        
        printf("T %i- x %li -> %lf\n", omp_get_thread_num(), x, fim-ini);
    }
      
    total += 1;
    
    printf ("Primos entre 1 e n: %d \n" , total);           

    
    return (0) ;
}
