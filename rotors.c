#include <stdbool.h>
#include <string.h>

#include "rotors.h"

const int ROTORCONFIGS[5][26] = {
    {
        4, 18, 14, 21, 15, 25, 9, 0,  24, 16, 20, 8,  17,
        7, 23, 11, 13, 5,  19, 6, 10, 3,  2,  12, 22, 1,
    },
    {
        21, 25, 1,  17, 6,  8,  19, 24, 20, 15, 18, 3, 13,
        7,  11, 23, 0,  22, 12, 9,  16, 14, 5,  4,  2, 10,
    },
    {
        9, 15, 6,  21, 14, 20, 12, 5,  24, 16, 1, 4,  13,
        7, 25, 17, 3,  10, 0,  18, 23, 11, 8,  2, 19, 22,
    },
    {
        13, 25, 9, 7, 6, 17, 2,  23, 12, 24, 18, 22, 1,
        14, 20, 5, 0, 8, 21, 11, 15, 4,  10, 16, 3,  19,
    },
    {
        5, 10, 16, 7, 19, 11, 23, 14, 2, 1,  9,  18, 15,
        3, 25, 17, 0, 12, 4,  22, 13, 8, 20, 24, 6,  21,
    }};

const int REFLECTORCONFIG[] = {24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14,
                               10, 12, 8,  4, 1,  5,  25, 2, 22, 21, 9,  0, 19};

// Notch positions for rotors 0-4 (where the stepping occurs)
const int ROTORNOTCHES[5] = {16, 4, 21, 9, 25};

void encode_char(char *input, int rotoroffsets[], int ringoffsets[],
                 int activerotors[], int rotorcount) {
  for (int i = 0; i < (int)strlen(input); i++) {
    increment_rotors(rotoroffsets, ringoffsets, activerotors, rotorcount);

    iteraterotors(rotoroffsets, ringoffsets, &input[i], true, activerotors,
                  rotorcount);

    reflector(&input[i]);

    iteraterotors(rotoroffsets, ringoffsets, &input[i], false, activerotors,
                  rotorcount);
  }
}

// TODO: Check function with other enigma machines
void runthrough(int rotoroffset, int ringoffset, char *input, bool forward,
                int activerotor) {
  *input = (*input) - 65;
  int rotorconf[26];
  memcpy(rotorconf, ROTORCONFIGS[activerotor], sizeof(rotorconf));
  // Apply ring offset to both the input and rotor offset
  int adjustedOffset = (rotoroffset - ringoffset + 26) % 26;
  if (forward == true) {
    *input = (*input + adjustedOffset) % 26;
    *input = (65 + rotorconf[(int)*input]);
  } else {
    for (int i = 0; i < 26; i++) {
      if (*input == rotorconf[i]) {
        int output = i - adjustedOffset;
        while (output < 0) {
          output = 26 + output;
        }
        output = output % 26;

        *input = (65 + output);
      }
    }
  }
}

void iteraterotors(int rotoroffsets[], int ringoffsets[], char *input,
                   bool forward, int activerotors[], int rotorcount) {
  if (forward) {
    // Forward pass: go through rotors in order
    for (int i = 0; i < rotorcount; i++) {
      runthrough(rotoroffsets[i], ringoffsets[i], input, forward,
                 activerotors[i]);
    }
  } else {
    // Backward pass: go through rotors in reverse order
    for (int i = rotorcount - 1; i >= 0; i--) {
      runthrough(rotoroffsets[i], ringoffsets[i], input, forward,
                 activerotors[i]);
    }
  }
}

void increment_rotors(int rotoroffsets[], int ringoffsets[], int activerotors[],
                      int rotorcount) {
  // First, advance the rightmost (fast) rotor
  rotoroffsets[0]++;
  if (rotoroffsets[0] >= 26) {
    rotoroffsets[0] = 0;
  }

  // Check for stepping based on notch positions adjusted by ring settings
  for (int i = 0; i < rotorcount - 1; i++) {
    // Notch position adjusted by ring setting
    int adjustedNotch =
        (ROTORNOTCHES[activerotors[i]] - ringoffsets[i] + 26) % 26;

    // If current rotor position equals adjusted notch, step the next rotor
    if (rotoroffsets[i] == adjustedNotch) {
      rotoroffsets[i + 1]++;
      if (rotoroffsets[i + 1] >= 26) {
        rotoroffsets[i + 1] = 0;
      }
    }
  }
}

void reflector(char *input) {
  if (*input >= 65 && *input <= 90) {
    (*input) = 65 + REFLECTORCONFIG[(*input) - 65];
  } else {
    *input = '?';
  }
}
