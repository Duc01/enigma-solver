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

// void encrypt_rotors(int rotorconf[], int rotorcount, char *input) {
//   if (!((*input >= 65 && *input <= 90) || (*input >= 97 && *input <= 122)))
//     return;
//   *input += rotorconf[0];
//   increment_rotor(rotorconf, rotorcount);
//
//   int iterarr[rotorcount - 1];
//   for (int i = 1; i < rotorcount; i++) {
//     iterarr[i - 1] = rotorconf[i];
//   }
//   if (rotorcount == 1)
//     return;
//
//   int iterarrlen = sizeof(iterarr) / sizeof(int);
//   encrypt_rotors(iterarr, iterarrlen, input);
// }
//
// // TODO: Function should increment next rotor in sequence by one when
// previous
// // one completes one revolution
// void increment_rotor(int rotorconf[], int rotorcount) {
//   for (int i = 0; i < rotorcount; i++) {
//     if (rotorconf[i] == 25) {
//       rotorconf[i] = 0;
//       if (i + 1 != rotorcount)
//         rotorconf[i + 1]++;
//     } else
//       rotorconf[i]++;
//   }
//   return;
// }
//
// void encrypt_str(int rotorconf[], int rotorcount, char *input) {
//   for (int i = 0; input[i] != '\0'; i++) {
//     encrypt_rotors(rotorconf, rotorcount, &input[i]);
//   }
// }

char encode_char(char input, int rotoroffsets[], int activerotors[],
                 int rotorcount) {
  increment_rotors(rotoroffsets, rotorcount);

  input = iteraterotors(rotoroffsets, input, true, activerotors, rotorcount);

  // Add reflector
  // Add backwards rotor pass
  return input;
}
// TODO: Check function with other enigma machines
char runthrough(int rotoroffset, char input, bool forward, int activerotor) {
  input = input - 65;
  int rotorconf[26];
  memcpy(rotorconf, ROTORCONFIGS[activerotor], sizeof(rotorconf));
  if (forward == true) {
    input = (input + rotoroffset) % 26;
    return (char)(65 + rotorconf[(unsigned char)input]);
  } else {
    for (int i = 0; i < 26; i++) {
      if (input == rotorconf[i]) {
        int output = i - rotoroffset;
        while (output < 0) {
          output = 26 + output;
        }
        output = output % 26;

        return (char)(65 + output);
      }
    }
  }

  return '?'; // just here to make the compiler shut up
}

char iteraterotors(int rotoroffsets[], char input, bool forward,
                   int activerotors[], int rotorcount) {
  for (int i = 0; i < rotorcount; i++) {
    input = runthrough(rotoroffsets[i], input, forward, activerotors[i]);
  }
  return input;
}

void increment_rotors(int rotoroffsets[], int rotorcount) {
  for (int i = 0; i < rotorcount; i++) {
    if (rotoroffsets[i] == 25) {
      rotoroffsets[i] = 0;
      if (i + 1 == rotorcount)
        rotoroffsets[i + 1]++;
    } else {
      rotoroffsets[i]++;
      break;
    }
  }
}
