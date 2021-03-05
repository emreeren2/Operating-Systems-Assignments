#include <stdio.h>
#include <stdlib.h>

int main()
{
  char chr;
  FILE *fp;

  fp = fopen("loremipsum.txt", "r");

  int counter = 0;
  while ((chr = fgetc(fp)) != EOF) {
    if (chr == 'a') {
      counter++;
    }
  }
  printf("%d\n", counter);

  return 0;
}
