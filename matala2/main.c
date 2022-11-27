#include <stdio.h>
#include <string.h>

// int main()
// {
//     int len=0;
// 	char str[] = "strtok needs to be called several times to split a string";
//     char* str2;
//     strcpy(str2, str);
// 	int init_size = strlen(str);
// 	char delim[] = " ";

// 	char *ptr = strtok(str, delim);
//     int i=0;
// 	while(ptr != NULL)
// 	{
//         len++;
//         // strcpy(ans[i], ptr);
// 		// printf("'%s'\n", ptr);
// 		ptr = strtok(NULL, delim);
// 	}
//     char* ans[] = (char** ) malloc(len);
//     char *ptr2 = strtok(str2, delim);
//     while(ptr2 != NULL)
// 	{
//         char* inans = (char*) malloc(strlen(ptr));
//         ans[i] = inans;
//         strcpy(ans[i], ptr);
// 		// printf("'%s'\n", ptr);
// 		ptr = strtok(NULL, delim);
// 	}

int main(){
    char* cmd = "strtok needs to be called several times to split a string";
    // char *token;
    // token = strtok(cmd, " ");
    // char* param[1024];
    // int i=0;
    // while( token != NULL ) {
    //     param[i] = token;
    //     token = strtok(NULL, " ");

    // }
    // param[i] = NULL;
    cmd+=2;
    printf("%s", cmd);
    
    return 0;
}


// int main()
// {
// 	char str[] = "ls re a s d f";
//     char cp[strlen(str)];
//     strcpy(cp, str);
//     char* a = strtok(cp, " ");
//     int len = (int)strlen(str) - (int)strlen(a);
//     char b[len];
//     int j=0;
//     for (size_t i = strlen(a); i < strlen(str); i++)
//     {
//         b[j] = str[i];
//         j++;
//     }
//     printf("%s\n", b);

// 	return 0;
// }
