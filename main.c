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
  // 15
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
  // TODO: Impose limitation of only uppercase characters

  Plugboard *pb = &(Plugboard){0};
  // getsettings(argv[1], pb);
  // printf("%s\n", pb->wiredchars);

  // int rotorsetup[] = {0, 1, 4};
  // int rotoroffsets[] = {0, 0, 0};
  // int ringoffsets[] = {0, 0, 0};
  // int rotorcount = 3;
  // parsesettings(argv[1], rotorsetup, rotoroffsets, pb);
  // printf("{%d, %d, %d}\n{%d, %d, %d}\n", rotorsetup[0], rotorsetup[1],
  //        rotorsetup[2], rotoroffsets[0], rotoroffsets[1], rotoroffsets[2]);
  //
  char *ciphertext = malloc(MAX_INPUT_LEN);
  parsetext(argv[2], ciphertext);
  // fixrotors(rotorsetup, ciphertext)
  rotorsetup possiblerotors[5] = {0};
  fixrotors(possiblerotors, ciphertext);

  // printf("{%d, %d, %d}\n", possiblerotors[0].rotor[0], rotorsetup[1],
  //        rotorsetup[2]);
  for (int i = 0; i < 5; i++) {
    printf("{%d, %d, %d}\n", possiblerotors[i].rotor[0],
           possiblerotors[i].rotor[1], possiblerotors[i].rotor[2]);
  }
  free(ciphertext);

  return 0;
}
