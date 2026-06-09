#include "plugboard.h"
#include "rotors.h"

#include <stdio.h>

// IMPOSE A INPUT CHARACTER LIMIT FOR INPUT TEXT AS strlen() returns type size_t
// and size_t -> int type conversion is unsafe above 32bit integer limit
int main(int argc, char **argv) {
  if (argc == 1 || argc >= 3) {
    fprintf(stderr, "Usage: [%s] filename.txt", argv[0]);
    return 1;
  }

  Plugboard pb = {0};

  int rotorsetup[] = {0, 0, 0};
  int rotorcount = 3;
  char test = 'A';
  printf("Rotor setup before: {%i, %i, %i}\n", rotorsetup[0], rotorsetup[1],
         rotorsetup[2]);
  encrypt_rotors(rotorsetup, rotorcount, &test);
  printf("%c\n", test);
  encrypt_rotors(rotorsetup, rotorcount, &test);
  printf("%c\n", test);
  printf("Rotor setup after: {%i, %i, %i}\n", rotorsetup[0], rotorsetup[1],
         rotorsetup[2]);

  return 0;
}
