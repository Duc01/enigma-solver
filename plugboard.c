#include "plugboard.h"

#include <stdio.h>
#include <string.h>

PlugboardError parse_plugboard(const char *input, Plugboard *pb) {
  if (!input || !pb) {
    fprintf(stderr, "Improper parameters for parse_plugboard function");
    return PLUGBOARD_ERR_NULL;
  }

  // pb->wiredchars initialised in alphabetical order
  for (int i = 0; i < 26; i++) {
    pb->wiredchars[i] = (char)('A' + i);
  }

  for (int i = 0, j = 0; i < (int)strlen(input); i++) {
    if (input[i] == 32)
      continue;

    if (pb->pairs[j].a <= 'Z' && pb->pairs[j].a >= 'A') {
      pb->pairs[j].b = input[i];
      j++;
    } else {
      pb->pairs[j].a = input[i];
    }
  }

  for (int i = 0; i < TOTAL_PAIRS; i++) {
    pb->wiredchars[pb->pairs[i].b - 'A'] = pb->pairs[i].a;
    pb->wiredchars[pb->pairs[i].a - 'A'] = pb->pairs[i].b;
  }

  return PLUGBOARD_OK;
}

void print_plugboard(Plugboard *pb) {
  if (!pb)
    return;

  printf("Plugboard: {\n");
  printf("    Wired: %s\n", pb->wiredchars);
  printf("    Pairs: ");

  for (int i = 0; i < TOTAL_PAIRS; i++) {
    printf("%c%c ", pb->pairs[i].a, pb->pairs[i].b);
  }
  printf("\n}\n");
}

void encrypt_plugboard(Plugboard *pb, char *input) {
  if (!pb)
    return;

  // size_t inputlen = strlen(input);
  // char *buffer = malloc(inputlen);
  for (int i = 0; i < (int)strlen(input); i++) {
    if (input[i] == 32)
      continue;
    if (input[i] >= 97 && input[i] <= 122)
      input[i] -= 32;
    // replace each character in input with character at the input[i] - 'A'
    // position of pb->wiredchars
    input[i] = pb->wiredchars[input[i] - 'A'];
  }
}
