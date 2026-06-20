#include <stdbool.h>
#include <string.h>

#include "plugboard.h"
#include "rotors.h"

void encrypt_str(char *input, int rotoroffsets[], int ringoffsets[],
                 int activerotors[], int rotorcount, Plugboard *pb) {
  encrypt_plugboard(pb, input);
  encode_char(input, rotoroffsets, ringoffsets, activerotors, rotorcount);
  encrypt_plugboard(pb, input);
}
