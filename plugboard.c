#include "plugboard.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

PlugboardError parse_plugboard(const char *input, Plugboard *pb) {
  if (!input || !pb)
    return PLUGBOARD_ERR_NULL;

  for (int i = 0; i < 26; i++) {
    pb->wiredchars[i] = (char)('A' + i);
  }

  int pair_count = 0;
  bool parsing_second_letter = false;

  for (int i = 0; i < (int)strlen(input); i++) {
    char c = input[i];
    if (c == ' ' || c == '\n' || c == '\r')
      continue;

    if (c < 'A' || c > 'Z')
      continue;

    if (pair_count >= TOTAL_PAIRS)
      break;

    if (parsing_second_letter) {
      pb->pairs[pair_count].b = c;
      pair_count++;
      parsing_second_letter = false;
    } else {
      pb->pairs[pair_count].a = c;
      parsing_second_letter = true;
    }
  }

  // Only wire up the pairs that were actually provided
  for (int i = 0; i < pair_count; i++) {
    pb->wiredchars[pb->pairs[i].b - 'A'] = pb->pairs[i].a;
    pb->wiredchars[pb->pairs[i].a - 'A'] = pb->pairs[i].b;
  }

  pb->wiredchars[26] = '\0';
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

  for (int i = 0; i < (int)strlen(input); i++) {
    // convert all applicable characters to uppercase
    if (input[i] >= 'a' && input[i] <= 'z') {
      input[i] -= 32;
    }

    if (input[i] >= 'A' && input[i] <= 'Z') {
      input[i] = pb->wiredchars[input[i] - 'A'];
    }
  }
}
