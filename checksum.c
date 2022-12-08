#include <stdio.h>
#include <stdlib.h>




int main(int argc, char *argv[]) {
  // Check the number of arguments
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }

  // Open the file
  FILE *file = fopen(argv[1], "rb");
  if (!file) {
    perror("fopen");
    return 1;
  }

  // Calculate the checksum
  unsigned char buffer[1024];
  size_t bytes_read;
  unsigned int checksum = 0;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    for (size_t i = 0; i < bytes_read; i++) {
      checksum += buffer[i];
    }
  }

  // Print the checksum
  printf("Checksum: %u\n", checksum);

  // Close the file
  if (fclose(file) != 0) {
    perror("fclose");
    return 1;
  }

  return 0;
}
