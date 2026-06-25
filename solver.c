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
  int textlen = (int)strlen(text);
  double coincidence = 0;
  for (int i = 0; i < textlen; i++) {
    double f = frequency(text[i], text);
    coincidence += (f * (f - 1));
  }
  coincidence /= (textlen * (textlen - 1));
  // normalising score by dividing by length of the alphabet
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

  int choices[5] = {0, 1, 2, 3, 4};
  int temprings[3] = {0, 0, 0}; // rings held at zero while setting rotors
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (i == j)
        continue;
      for (int k = 0; k < 5; k++) {
        if (k == i || k == j)
          continue;
        int rotors[3] = {i, j, k};
        printf("Trying: %d %d %d\n", rotors[0], rotors[1], rotors[2]);

        for (int a = 0; a < 26; a++) {
          for (int b = 0; b < 26; b++) {
            for (int c = 0; c < 26; c++) {
              int currentoffsets[3] = {a, b, c};
              char *input = malloc(strlen(ciphertext) + 1);
              if (input == NULL)
                return;

              strcpy(input, ciphertext);
              encode_char(input, currentoffsets, temprings, rotors, 3);
              double fit = indexofcoincidence(input);
              free(input);

              // looping over possible rotors to replace worse scores
              for (int i = 0; i < 5; i++) {
                if (possiblerotors[i].score < fit) {
                  possiblerotors[i].score = fit;
                  memcpy(possiblerotors[i].rotor, rotors,
                         sizeof(possiblerotors[i].rotor));
                  memcpy(possiblerotors[i].offsets, currentoffsets,
                         sizeof(possiblerotors[i].offsets));
                  break;
                }
              }
            }
          }
        }
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
