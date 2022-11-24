#include "codec.h"

char* encode(char *src, char *dst, int len)
{
    dst = (char *)(malloc(len));
    for (size_t i = 0; i < len; i++)
    {
        dst[i] = tolower(src[i]);
    }
    dst[len] = '\0';
    printf("%s\n", dst);
    return dst;
}

char *decode(char *src, char *dst, int len)
{
    dst = (char *)(malloc(len));
    for (size_t i = 0; i < len; i++)
    {
        dst[i] = toupper(src[i]);
    }
    dst[len] = '\0';
    printf("%s\n", dst);
    return dst;
}
