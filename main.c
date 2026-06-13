#include <stdbool.h>
#include <stdio.h>

#include "rotors.h"

// IMPOSE A INPUT CHARACTER LIMIT FOR INPUT TEXT AS strlen() returns type size_t
// and size_t -> int type conversion is unsafe above 32bit integer limit
int main(int argc, char **argv) {
  if (argc == 1 || argc >= 3) {
    fprintf(stderr, "Usage: [%s] filename.txt", argv[0]);
    return 1;
  }
  // TODO: Impose limitation of only uppercase characters

  // Plugboard pb = {0};

  int rotorsetup[] = {0, 1, 2};
  int rotoroffsets[] = {0, 0, 0};
  int ringoffsets[] = {0, 0, 0};
  int rotorcount = 3;
  char input[] = "GDXVL";
  printf("Original str: %s\n", input);
  printf("Rotor offsets: {%d, %d, %d}\n", rotoroffsets[0], rotoroffsets[1],
         rotoroffsets[2]);
  printf("Ring offsets: {%d, %d, %d}\n", ringoffsets[0], ringoffsets[1],
         ringoffsets[2]);

  encode_char(input, rotoroffsets, ringoffsets, rotorsetup, rotorcount);
  printf("AFter rotor manipulation: %s\n", input);
  printf("Later rotor offset: {%d, %d, %d}\n", rotoroffsets[0], rotoroffsets[1],
         rotoroffsets[2]);
  printf("Later ring offset: {%d, %d, %d}\n", ringoffsets[0], ringoffsets[1],
         ringoffsets[2]);
  return 0;
}
