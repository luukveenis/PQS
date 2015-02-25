#include <stdio.h>
#include <string.h>

void process_customers(FILE *fin){
  char buf[100];
  int len;

  while (fgets(buf, 100, fin)){
    len = strlen(buf);
    if (buf[len-1] == '\n') buf[len-1] = '\0';
    printf("%s\n", buf);
  }
}
