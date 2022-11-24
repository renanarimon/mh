#include <stdio.h>
#include <string.h>

int main()
{
	char str[] = "strtok needs to be called several times to split a string";
	int init_size = strlen(str);
	char delim[] = " ";

	char *ptr = strtok(str, delim);
    char* ans[9];
    int i=0;
	while(ptr != NULL)
	{
        ans[i] = ptr;
		ptr = strtok(NULL, delim);
        i++;
	}
    
    
	return 0;
}
