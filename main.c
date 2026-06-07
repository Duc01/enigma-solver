#include <stdio.h>

int main(int argc, char **argv) {
  if (argc == 1 || argc >= 3) {
    fprintf(stderr, "Usage: [%s] filename.txt", argv[0]);
    return 1;
  }

  else
    printf("%s %s\n", argv[0], argv[1]);
  printf("Shut yo bitchass up");
  return 0;
}
