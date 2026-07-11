#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encode.h"
#include "plugboard.h"
#include "rotors.h"
#include "solver.h"

#define MAX_INPUT_LEN 65535

char *trimstart(char *str) {
  if (str == NULL)
    return NULL;

  while (isspace((unsigned char)*str)) {
    str++; // Looks a bit unclean but idk dude
  }
  return str;
}

void parsetext(char *filepath, char *inputstr) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    perror("Can't find file text input file");
    return;
  }

  char inputbuf[MAX_INPUT_LEN];
  if (fgets(inputbuf, sizeof(inputbuf), file) == NULL) {
    perror("Couldn't read from text file");
    fclose(file);
    return;
  }
  fclose(file);
  char *trimmed = trimstart(inputbuf);
  strcpy(inputstr, trimmed);
}

void parsesettings(char *filepath, int rotorsetup[], int rotoroffsets[],
                   Plugboard *pb) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    perror("Invalid file path");
    return;
  }
  char inputstr[100]; // arbitrary length
  if (fgets(inputstr, sizeof(inputstr), file) == NULL) {
    perror("Couldn't read from file");
    fclose(file);
    return;
  }
  char *rotorcfgstr = trimstart(inputstr);

  int n;
  if (sscanf(rotorcfgstr, "%d %d %d %d %d %d %n", &rotorsetup[0],
             &rotorsetup[1], &rotorsetup[2], &rotoroffsets[0], &rotoroffsets[1],
             &rotoroffsets[2], &n) != 6) {
    fprintf(stderr, "Invalid rotor settings\n");
    fclose(file);
    return;
  }

  fclose(file);
  char *plugboardinput = &rotorcfgstr[15];
  parse_plugboard(plugboardinput, pb);
  print_plugboard(pb);
}

// IMPOSE A INPUT CHARACTER LIMIT FOR INPUT TEXT AS strlen() returns type size_t
// and size_t -> int type conversion is unsafe above 32bit integer limit
int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: [%s] settingsfile.txt input.txt", argv[0]);
    return 1;
  }
  char *ciphertext = malloc(MAX_INPUT_LEN);
  parsetext(argv[2], ciphertext);
  const int CIPHERLEN = strlen(ciphertext);

  for (int i = 0; i < CIPHERLEN; i++) {
    if ((ciphertext[i] < 'A' || ciphertext[i] > 'Z') &&
        (ciphertext[i] < 'a' || ciphertext[i] > 'z')) {
      if (ciphertext[i] == '\r' || ciphertext[i] == '\n')
        continue;
      fprintf(stderr, "Input must contain only the english alphabet\n");
      return 2;
    }
    if (ciphertext[i] >= 'a' && ciphertext[i] <= 'z')
      ciphertext[i] -= 32;
  }

  printf("%s\n", ciphertext);
  RotorSetup possiblerotors[5];
  fixrotors(possiblerotors, ciphertext);
  printf("\n--- TOP 5 ROTOR SETUPS FOUND BY SOLVER ---\n");
  for (int i = 0; i < 5; i++) {
    printf("Rank %d: Rotors: %d %d %d, Offsets: %d %d %d, Score: %lf\n", i + 1,
           possiblerotors[i].rotor[0], possiblerotors[i].rotor[1],
           possiblerotors[i].rotor[2], possiblerotors[i].offsets[0],
           possiblerotors[i].offsets[1], possiblerotors[i].offsets[2],
           possiblerotors[i].score);
  }
  printf("------------------------------------------\n");

  PlugSetup plugs[20] = {0};
  fixplugs(plugs, ciphertext, &possiblerotors[0]);
  free(ciphertext);

  return 0;
}
