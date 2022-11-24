#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void copy(const char *fileName1, const char *fileName2)
{
  char buf[1];
  int outf, inf, fileread;

  if ((outf = open(fileName2, O_CREAT | O_WRONLY)) == -1)
  {
    perror("Usage : copy <file1> <file2>");
    exit(0);
  }

  if ((inf = open(fileName1, O_RDONLY)) == -1)
  {
    perror("Usage : copy <file1> <file2>");
    exit(0);
  }

  char s[128] = "> ";
  strcat(s, fileName2);
  system(s);

  if (inf > 0)
  {
    fileread = read(inf, buf, 1);

    while (fileread)
    {
      write(outf, buf, 1);
      fileread = read(inf, buf, 1);
    }

    close(inf);
  }
  close(outf);
  printf("file is copied\n");
}

int main(int argc, char *argv[])
{
  if ((argc == 3)) // get into the link and copy cont
  {
    copy(argv[1], argv[2]);
  }
  else if (argc == 4 && strcmp(argv[1], "-l")==0) // if link - copy link, else - copy content
  {
    struct stat s;
    if ((lstat(argv[2], &s)) < 0)
    {
      perror("error lstat");
      exit(0);
    }
    if ((S_ISLNK(s.st_mode)) == 0)
    { // not link
      copy(argv[2], argv[3]);
    }
    else if ((S_ISLNK(s.st_mode)) == 1)
    { // link
      char buf[128];
      int fileread, outf;
      
      if ((outf = open(argv[3], O_CREAT | O_WRONLY)) == -1)
      {
        perror("Usage : copy <file1> <file2>");
      }
      char s[128] = "> ";
      strcat(s, argv[3]);
      system(s);
      if ((fileread = readlink(argv[2], buf, 128)) == -1)
      {
        perror("error read link");
        exit(0);
      }
      write(outf, buf, strlen(buf));
      close(outf);
      printf("link was copied!");
    }
  }
  
  return 0;
}