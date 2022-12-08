#include <stdio.h>
#include<time.h>

void create100MBfile(char *fileName)
{
    clock_t startCreateFile = clock();
    // Create a buffer to hold the data
    const int BUFFER_SIZE = 1024 * 1024 * 100; // 100 MB
    char *buffer = malloc(BUFFER_SIZE * sizeof(char));
    if (!buffer)
    {
        perror("malloc");
        return 1;
    }

    // Fill the buffer with random characters
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = rand() % 256;
    }

    // Open the file
    FILE *file = fopen(fileName, "wb");
    if (!file)
    {
        perror("fopen");
        return 1;
    }

    // Write the data to the file
    size_t bytes_written = fwrite(buffer, sizeof(char), BUFFER_SIZE, file);
    if (bytes_written != BUFFER_SIZE)
    {
        fprintf(stderr, "Error writing to file\n");
        return 1;
    }

    // Close the file
    if (fclose(file) != 0)
    {
        perror("fclose");
        return 1;
    }

    // Free the buffer
    free(buffer);
    clock_t endCreateFile = clock();
    double timeToken = (double)(endCreateFile - startCreateFile) / CLOCKS_PER_SEC;
    printf("time to create 100MB file %s: %f secs\n",fileName ,timeToken);
}

int checkSum(int argc, char *argv[])
{
    clock_t startCheckSum = clock();
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Open the file
    FILE *file = fopen(argv[1], "rb");
    if (!file)
    {
        perror("fopen");
        return 1;
    }

    // Calculate the checksum
    unsigned char buffer[1024];
    size_t bytes_read;
    unsigned int checksum = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            checksum += buffer[i];
        }
    }

    // Print the checksum
    printf("Checksum: %u\n", checksum);

    // Close the file
    if (fclose(file) != 0)
    {
        perror("fclose");
        return 1;
    }
    clock_t endCheckSum = clock();
    double timeToken = (double)(endCheckSum - startCheckSum) / CLOCKS_PER_SEC;
    printf("time to checkSum file %s: %f secs\n", argv[1] ,timeToken);

    return 0;
}

int main(int argc, char *argv[])
{
    create100MBfile("f.txt");
    checkSum(argc, argv);

    return 0;
}
