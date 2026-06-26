#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encode.h"
#include "plugboard.h"
#include "rotors.h"
#include "solver.h"

int frequency(char target, const char *text) {
  int f = 0;
  for (int i = 0; i < (int)strlen(text); i++) {
    if (target == text[i])
      f++;
  }
  return f;
}

double indexofcoincidence(const char *text) {
  int textlen = 0;
  int counts[26] = {0};
  for (int i = 0; text[i] != '\0'; i++) {
    if (text[i] >= 'A' && text[i] <= 'Z') {
      counts[text[i] - 'A']++;
      textlen++;
    }
  }
  if (textlen <= 1)
    return 0.0;
  double coincidence = 0;
  for (int i = 0; i < 26; i++) {
    coincidence += (double)counts[i] * (counts[i] - 1);
  }
  coincidence /= ((double)textlen * (textlen - 1));
  return coincidence;
}

void fixrotors(rotorsetup possiblerotors[5], const char *ciphertext) {

  // setting up possiblerotors before execution
  for (int i = 0; i < 5; i++) {
    possiblerotors[i].rotor[0] = 0;
    possiblerotors[i].rotor[1] = 1;
    possiblerotors[i].rotor[2] = 2;
    possiblerotors[i].offsets[0] = 0;
    possiblerotors[i].offsets[1] = 0;
    possiblerotors[i].offsets[2] = 0;

    possiblerotors[i].rings[0] = 0;
    possiblerotors[i].rings[1] = 0;
    possiblerotors[i].rings[2] = 0;

    possiblerotors[i].score = -1e12;
  }

  int temprings[3] = {0, 0, 0}; // rings held at zero while setting rotors
  for (int r0 = 0; r0 < 5; r0++) {
    for (int r1 = 0; r1 < 5; r1++) {
      if (r0 == r1)
        continue;
      for (int r2 = 0; r2 < 5; r2++) {
        if (r2 == r0 || r2 == r1)
          continue;
        int rotors[3] = {r0, r1, r2};
        printf("Trying: %d %d %d\n", rotors[0], rotors[1], rotors[2]);

        char *input = malloc(strlen(ciphertext) + 1);
        if (input == NULL)
          return;

        for (int a = 0; a < 26; a++) {
          for (int b = 0; b < 26; b++) {
            for (int c = 0; c < 26; c++) {
              int currentoffsets[3] = {a, b, c};

              strcpy(input, ciphertext);
              encode_char(input, currentoffsets, temprings, rotors, 3);
              double fit = indexofcoincidence(input);

              // insert into sorted top 5 array
              for (int idx = 0; idx < 5; idx++) {
                if (fit > possiblerotors[idx].score) {
                  // Shift elements down
                  for (int l = 4; l > idx; l--) {
                    possiblerotors[l] = possiblerotors[l - 1];
                  }
                  possiblerotors[idx].score = fit;
                  memcpy(possiblerotors[idx].rotor, rotors,
                         sizeof(possiblerotors[idx].rotor));
                  memcpy(possiblerotors[idx].offsets, currentoffsets,
                         sizeof(possiblerotors[idx].offsets));
                  break;
                }
              }
            }
          }
        }
        free(input);
        printf("Fit: %lf\n", possiblerotors[0].score);
      }
    }
  }
}

// void fixrotors(int rotorsetup[3], char *ciphertext) {
//   double best_fitness = -1e12;
//
//   int rotoroffsets[3] = {0, 1, 2};
//   int ringoffsets[3] = {0, 0, 0}; // held at zero during rotor search
//   // activerotors == rotorsetup
//   int rotorcount = 3;
//
//   int choices[5] = {0, 1, 2, 3, 4};
//
//   for (int i = 0; i < 5; i++) {
//     for (int j = 0; j < 5; j++) {
//       if (j == i)
//         continue;
//       for (int k = 0; k < 5; k++) {
//         if (k == i || k == j)
//           continue;
//         int rotors[3] = {i, j, k};
//         printf("Checking %d %d %d\n", i, j, k);
//
//         for (int a = 0; a < 26; a++) {
//           for (int b = 0; b < 26; b++) {
//             for (int c = 0; c < 26; c++) {
//               int currentoffsets[3] = {a, b, c};
//
//               char *input = malloc(strlen(ciphertext) + 1);
//               if (input == NULL) {
//                 perror("Couldn't assign memory\n");
//                 return;
//               }
//               strcpy(input, ciphertext);
//
//               // prefer using encode_char here instead of encrypt plugboard
//               // because plugboard doesn't yet exist this shouldn't affect
//               much encode_char(input, currentoffsets, ringoffsets, rotors,
//                           rotorcount);
//
//               double fit = indexofcoincidence(input);
//               free(input);
//
//               if (fit > best_fitness) {
//                 best_fitness = fit;
//                 memcpy(rotoroffsets, currentoffsets, sizeof(rotoroffsets));
//                 // rotoroffsets[0] = a;
//                 // rotoroffsets[1] = b;
//                 // rotoroffsets[2] = c;
//                 memcpy(rotorsetup, rotors, sizeof(rotorsetup));
//                 // rotorsetup[0] = i;
//                 // rotorsetup[1] = j;
//                 // rotorsetup[2] = k;
//               }
//             }
//           }
//         }
//       }
//     }
//   }
// }

// void fixplugs()
