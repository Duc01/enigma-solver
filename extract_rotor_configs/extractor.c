#include <stdio.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: [%s] rotorconfig", argv[0]);
    return 1;
  }

  char *inputstr = argv[1];
  printf("{");
  for (int i = 0; i < 26; i++) {
    if (inputstr[i] < 65 || inputstr[i] > 97) {
      fprintf(stderr, "Invalid character at position %d", i);
      return 1;
    }

    printf("%d, ", inputstr[i] - 65);
  }
  printf("}\n");

  return 0;
}
