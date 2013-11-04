#include "stdio.h"
#include "stdlib.h"

int main()
{
    float * x = malloc(sizeof(float)*10);
    int i;
    for( i=0;i<10;i++)
    {
        x[i] = i;        
    }
    for(  i=0;i<10;i++)
    {
        printf("%f\n",x[i]);
    }
    return 0;
}
