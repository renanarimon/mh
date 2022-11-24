#include <stdio.h>
#include <string.h>



int main()
{
	char str[] = "strtok needs to be called several times to split a string";
    char cp[strlen(str)];
    strcpy(cp, str);
    char* a = strtok(cp, " ");
    int len = (int)strlen(str) - (int)strlen(a);
    char b[len];
    int j=0;
    for (size_t i = strlen(a); i < strlen(str); i++)
    {
        b[j] = str[i];
        j++;
    }

	return 0;
}
