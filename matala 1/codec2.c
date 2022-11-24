#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int addOne(int x)
{
    int m = 1;
    while( x & m )
    {
        x = x ^ m;
        m <<= 1;
    }   
    x = x ^ m;
    return x;
}

int subtractOne(int x)
{
    int m = 1;
    while (!(x & m)) {
        x = x ^ m;
        m <<= 1;
    }
    x = x ^ m;
    return x;
}

char *encode(char *src, char *dst, int len)
{
    dst = (char *)(malloc(len));
    int x;
    for (size_t i = 0; i < len; i++)
    {
        x = (int)src[i];
        x = ~x;
        x = addOne(x); 
        char y = x;
        strncat(dst, &y, 1);
        
    }
    dst[len] = '\0';
    printf("%s", dst);
    return dst;
}

char *decode(char *src, char *dst, int len)
{
    dst = (char *)(malloc(len));
    int x;
    for (size_t i = 0; i < len; i++)
    {
        x = (int)src[i];
        x = subtractOne(x);  
        x = ~x;
        char y = x;
        strncat(dst, &y, 1);
    }
    dst[len] = '\0';
    printf("%s", dst);
    return dst;
}

