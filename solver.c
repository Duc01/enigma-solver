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

// TODO: Implement bigram/tigram scoring functions for the plugboard
// IOC is too weak of a test for the plugboard to work properly
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

void fixrotors(RotorSetup possiblerotors[5], const char *ciphertext) {

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

void printplugs(PlugSetup plugs[], const int pluglen) {
  for (int i = 0; i < pluglen; i++) {
    printf("Pair: %c%c Score: %lf\n", plugs[i].plug.a, plugs[i].plug.b,
           plugs[i].score);
  }
}

void fixplugs(PlugSetup plugs[20], const char *ciphertext) {
  // array of commonly used plugboard combinations
  const char *ISTECKER[135] = {
      "AE", "AI", "AN", "AR", "AS", "AX", "BE", "BI", "BN", "BR", "BS", "BX",
      "CE", "CI", "CN", "CR", "CS", "CX", "DE", "DI", "DN", "DR", "DS", "DX",
      "EF", "EG", "EH", "EI", "EJ", "EK", "EL", "EM", "EN", "EO", "EP", "EQ",
      "ER", "ES", "ET", "EU", "EV", "EW", "EX", "EY", "EZ", "FI", "FN", "FR",
      "FS", "FX", "GI", "GN", "GR", "GS", "GX", "HI", "HN", "HR", "HS", "HX",
      "IJ", "IK", "IL", "IM", "IN", "IO", "IP", "IQ", "IR", "IS", "IT", "IU",
      "IV", "IW", "IX", "IY", "IZ", "JN", "JR", "JS", "JX", "KN", "KR", "KS",
      "KX", "LN", "LR", "LS", "LX", "MN", "MR", "MS", "MX", "NO", "NP", "NQ",
      "NR", "NS", "NT", "NU", "NV", "NW", "NX", "NY", "NZ", "OR", "OS", "OX",
      "PR", "PS", "PX", "QR", "QS", "QX", "RS", "RT", "RU", "RV", "RW", "RX",
      "RY", "RZ", "ST", "SU", "SV", "SW", "SX", "SY", "SZ", "TX", "UX", "VX",
      "WX", "XY", "XZ"};
  for (int i = 0; i < 20; i++) {
    plugs[i].plug.a = '\0';
    plugs[i].plug.b = '\0';
  }
  const int steckerlen = sizeof(ISTECKER) / sizeof(ISTECKER[0]);
  for (int i = 0; i < steckerlen; i++) {
    char *temptext = malloc(strlen(ciphertext) + 1);
    strcpy(temptext, ciphertext);
    Plugboard pb = {0};
    parse_plugboard(ISTECKER[i], &pb);
    encrypt_plugboard(&pb, temptext);
    double pairscore = indexofcoincidence(temptext);

    for (int j = 0; j < 20; j++) {
      if (plugs[j].plug.a == '\0' && plugs[j].plug.b == '\0') {
        plugs[j].plug.a = ISTECKER[i][0];
        plugs[j].plug.b = ISTECKER[i][1];
        plugs[j].score = pairscore;
        break;
      } else if ((plugs[j].plug.a == '\0' && plugs[j].plug.b != '\0') ||
                 (plugs[j].plug.a != '\0' && plugs[j].plug.b == '\0')) {
        fprintf(stderr, "Error testing plugboard configurations");
        return;
      }
      if (plugs[j].score < pairscore) {
        plugs[j].plug.a = ISTECKER[i][0];
        plugs[j].plug.b = ISTECKER[i][1];
        plugs[j].score = pairscore;
        break;
      }
    }
  }
  printplugs(plugs, 20);
}
