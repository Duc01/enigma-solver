#include <stdbool.h>
#include <string.h>

#include "rotors.h"

const int ROTORCONFIGS[5][26] = {
    {4,  10, 12, 5,  11, 6,  3,  16, 21, 25, 13, 19, 14,
     22, 24, 7,  23, 20, 18, 15, 0,  8,  1,  17, 2,  9}, // Rotor I
    {0,  9,  3, 10, 18, 8,  17, 20, 23, 1, 11, 7,  22,
     19, 12, 2, 16, 6,  25, 13, 15, 24, 5, 21, 14, 4}, // Rotor II
    {1,  3,  5, 7, 9,  11, 2, 15, 17, 19, 23, 21, 25,
     13, 24, 4, 8, 22, 6,  0, 10, 12, 20, 18, 16, 14}, // Rotor III
    {4, 18, 14, 21, 15, 25, 9, 0,  24, 16, 20, 8,  17,
     7, 23, 11, 13, 5,  19, 6, 10, 3,  2,  12, 22, 1}, // Rotor IV
    {21, 25, 1,  17, 6,  8,  19, 24, 20, 15, 18, 3, 13,
     7,  11, 23, 0,  22, 12, 9,  16, 14, 5,  4,  2, 10} // Rotor V
};

const int REFLECTORCONFIG[] = {24, 17, 20, 7,  16, 18, 11, 3, 15,
                               23, 13, 6,  14, 10, 12, 8,  4, 1,
                               5,  25, 2,  22, 21, 9,  0,  19}; // UKW‑B

// Notch positions (where stepping occurs) for rotors I–V
const int ROTORNOTCHES[5] = {16, 4, 21, 9, 25}; // Q, E, V, J, Z

void encode_char(char *input, int rotoroffsets[], int ringoffsets[],
                 int activerotors[], int rotorcount) {
  for (int i = 0; i < (int)strlen(input); i++) {
    // Only process uppercase letters
    if (input[i] < 'A' || input[i] > 'Z')
      continue;

    // Convert from ASCII letter to 0‑25
    input[i] -= 'A';

    // Advance the rotors (key press)
    increment_rotors(rotoroffsets, ringoffsets, activerotors, rotorcount);

    // Forward through rotors
    iteraterotors(rotoroffsets, ringoffsets, &input[i], true, activerotors,
                  rotorcount);

    // Reflector
    reflector(&input[i]);

    // Backward through rotors (in reverse order)
    iteraterotors(rotoroffsets, ringoffsets, &input[i], false, activerotors,
                  rotorcount);

    // Convert back to ASCII uppercase
    input[i] += 'A';
  }
}

void runthrough(int rotoroffset, int ringoffset, char *input, bool forward,
                int activerotor) {
  int val = *input; // 0‑25
  const int *rotorconf = ROTORCONFIGS[activerotor];

  // Combined offset: rotor position minus ring setting (normalised)
  int adjustedOffset = (rotoroffset - ringoffset + 26) % 26;

  if (forward) {
    // Apply offset, traverse core mapping, remove offset
    int pinIn = (val + adjustedOffset) % 26;
    int pinOut = rotorconf[pinIn];
    *input = (pinOut - adjustedOffset + 26) % 26;
  } else {
    // Apply offset, find inverse mapping, remove offset
    int pinIn = (val + adjustedOffset) % 26;
    int i;
    for (i = 0; i < 26; i++) {
      if (rotorconf[i] == pinIn)
        break;
    }
    *input = (i - adjustedOffset + 26) % 26;
  }
}

void iteraterotors(int rotoroffsets[], int ringoffsets[], char *input,
                   bool forward, int activerotors[], int rotorcount) {
  if (forward) {
    for (int i = 0; i < rotorcount; i++) {
      runthrough(rotoroffsets[i], ringoffsets[i], input, true, activerotors[i]);
    }
  } else {
    for (int i = rotorcount - 1; i >= 0; i--) {
      runthrough(rotoroffsets[i], ringoffsets[i], input, false,
                 activerotors[i]);
    }
  }
}

void increment_rotors(int rotoroffsets[], int ringoffsets[], int activerotors[],
                      int rotorcount) {
  bool step_middle = false;
  bool step_left = false;

  // Check notch states BEFORE moving anything
  if (rotorcount > 1) {
    int rightnotch = (ROTORNOTCHES[activerotors[0]] - ringoffsets[0] + 26) % 26;
    if (rotoroffsets[0] == rightnotch) {
      step_middle = true;
    }
  }

  // Double stepping anomaly
  if (rotorcount > 2) {
    int middlenotch =
        (ROTORNOTCHES[activerotors[1]] - ringoffsets[1] + 26) % 26;
    if (rotoroffsets[1] == middlenotch) {
      step_middle = true;
      step_left = true;
    }
  }

  rotoroffsets[0] = (rotoroffsets[0] + 1) % 26;

  if (step_middle) {
    rotoroffsets[1] = (rotoroffsets[1] + 1) % 26;
  }

  if (step_left) {
    rotoroffsets[2] = (rotoroffsets[2] + 1) % 26;
  }
}

// Reflector (UKW‑B).
void reflector(char *input) {
  if ((unsigned char)*input < 26) {
    *input = REFLECTORCONFIG[(int)*input];
  } else {
    *input = -1; // Should never happen if the input is valid
  }
}
