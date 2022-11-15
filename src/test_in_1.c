#include <stdio.h>
#include "FA.h"

double func(int a,char c)
{
    for(int b = 0;b<100;b++)
    {
        printf("%d %c",a,c);
    }
    return -1.23;
}

int main()
{
    func(3,'A');
    return 0;
}