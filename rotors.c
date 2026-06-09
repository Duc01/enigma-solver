#include "rotors.h"

void encrypt_rotors(int setup[], int setuplen, char *input) {
  *input += setup[0];
  increment_rotor(&setup[0]);

  int iterarr[setuplen - 1];
  for (int i = 1; i < setuplen; i++) {
    iterarr[i - 1] = setup[i];
  }
  if (setuplen == 1)
    return;

  int iterarrlen = sizeof(iterarr) / sizeof(int);
  encrypt_rotors(iterarr, iterarrlen, input);
}

// TODO: Function should increment next rotor in sequence by one when previous
// one completes one revolution
void increment_rotor(int *config) {
  if (*config == 25)
    *config = 0;
  else
    (*config)++;
  return;
}
