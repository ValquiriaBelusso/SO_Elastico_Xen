#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// como executar
//gcc -o teste1 teste1_seq.c -lm
//./teste1 100000

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

    if ( argc < 2)
    {
        printf("Valor invalido! Entre com o valor do maior inteiro \n") ;
        return 0;
    }
    else
    {
        n = strtol(argv[1], (char **) NULL , 10) ;
    }
    
    for ( i = 3; i <= n ; i += 2)
        if ( primo ( i ) == 1) total ++;
           total += 1;
           
    printf ("Primos entre 1 e n: %d \n" , total);
    
    return (0) ;
}
