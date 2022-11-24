#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void comp(FILE *f1, FILE *f2)
{
    if (f1 == NULL || f2 == NULL)
    {
        printf("Usage : cmp <file1> <file2>");
        exit(0);
    }

    char ch1 = getc(f1);
    char ch2 = getc(f2);
    if (ch1 != EOF || ch2 != EOF)
    {
        printf("Not identical\n");
        return;
    }

    while (ch1 != EOF && ch2 != EOF)
    {
        if (ch1 != ch2)
        {
            printf("Not identical\n");
            return;
        }
        ch1 = getc(f1);
        ch2 = getc(f2);
    }
    printf("Identical\n");
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage : cmp <file1> <file2>");
        exit(0);
    }
    FILE *f1 = fopen(argv[1], "r");
    FILE *f2 = fopen(argv[2], "r");
    comp(f1, f2);
    fclose(f1);
    fclose(f2);
    return 0;
}