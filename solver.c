#include <string.h>

#include "solver.h"

int frequency(char target, char *text) {
  int f = 0;
  for (int i = 0; i < (int)strlen(text); i++) {
    if (target == text[i])
      f++;
  }
  return f;
}

double indexofcoincidence(char *text) {
  int textlen = (int)strlen(text);
  double coincidence = 0;
  for (int i = 0; i < textlen; i++) {
    double f = frequency(text[i], text);
    coincidence += (f * (f - 1));
  }
  coincidence /= (textlen * (textlen - 1));
  // normalising score by dividing by length of the alphabet
  coincidence /= 26;
  return coincidence;
}

void fixrotors(int rotorsetup[]) {}
