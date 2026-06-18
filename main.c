#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "plugboard.h"
#include "rotors.h"

void parseinput(char *filepath, int rotorsetup[], int rotoroffsets[],
                Plugboard *pb) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    perror("Invalid file path");
  }
  // TODO: Trim whitespace before handling string
  char inputstr[100]; // arbitrary length
  if (fgets(inputstr, sizeof(inputstr), file) == NULL) {
    perror("Couldn't read from file");
  }
  sscanf(inputstr, "%d %d %d %d %d %d", &rotorsetup[0], &rotorsetup[1],
         &rotorsetup[2], &rotoroffsets[0], &rotoroffsets[1], &rotoroffsets[2]);

  fclose(file);
  // 15
  char *plugboardinput = &inputstr[15];
  parse_plugboard(plugboardinput, pb);
  print_plugboard(pb);
}

// IMPOSE A INPUT CHARACTER LIMIT FOR INPUT TEXT AS strlen() returns type size_t
// and size_t -> int type conversion is unsafe above 32bit integer limit
int main(int argc, char **argv) {
  if (argc == 1 || argc >= 3) {
    fprintf(stderr, "Usage: [%s] filename.txt", argv[0]);
    return 1;
  }
  // TODO: Impose limitation of only uppercase characters

  Plugboard *pb = &(Plugboard){0};
  // getsettings(argv[1], pb);
  // printf("%s\n", pb->wiredchars);

  int rotorsetup[] = {0, 0, 0};
  int rotoroffsets[] = {0, 0, 0};
  int ringoffsets[] = {0, 0, 0};
  int rotorcount = 3;
  parseinput(argv[1], rotorsetup, rotoroffsets, pb);
  printf("{%d, %d, %d}\n{%d, %d, %d}\n", rotorsetup[0], rotorsetup[1],
         rotorsetup[2], rotoroffsets[0], rotoroffsets[1], rotoroffsets[2]);

  return 0;
}
