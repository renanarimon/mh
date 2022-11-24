#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "codec.h"

int main(int argc, char const *argv[])
{
    void *handle;
    void (*func_encode)(const char *src, char* dst , int len);

    if (strcmp(argv[1], "codec1") == 0)
    {
        handle = dlopen("./codec1.so", RTLD_LAZY);
    }
    else if (strcmp(argv[1], "codec2") == 0)
    {
        handle = dlopen("./codec2.so", RTLD_LAZY);
    }
    else
    {
        printf("error");
        exit(0);
    }
    if(!handle){
        printf("error loding libary");
        exit(0);
    }
    *(void**)(&func_encode)=dlsym(handle,"encode");
    if(!func_encode){
        printf("error");
        exit(0);
    }
    
    int len = strlen(argv[2]);
    char dest[len];
    func_encode(argv[2], dest, len);

    dlclose(handle);
    return 0;
}
