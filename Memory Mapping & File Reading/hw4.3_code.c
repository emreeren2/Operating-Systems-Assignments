#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
  int fd = open("loremipsum.txt", O_RDONLY);
  struct stat s;
  size_t size;
  int status = fstat(fd, &s);
  size = s.st_size;

  char *ptr = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  int counter = 0;
  for (size_t i = 0; i < size; i++) {
    if (ptr[i] == 'a') {
      counter++;
    }
  }
  printf("%d\n", counter);

  status = munmap(ptr, size);
  return 0;
}
