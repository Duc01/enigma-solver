#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encode.h"
#include "plugboard.h"
#include "rotors.h"
#include "solver.h"

#define NGRAM_SPACE 17576
#define FLOOR_FREQ 0.000010

static double str_freqs[NGRAM_SPACE];
static int str_freqs_loaded = 0;

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

// base-26 based hash function. Should be a perfectly flat distribution
unsigned int ngramhash(const char *sample) {
  unsigned int c0 = (sample[0] >= 'A' && sample[0] <= 'Z') ? sample[0] - 'A' : 0;
  unsigned int c1 = (sample[1] >= 'A' && sample[1] <= 'Z') ? sample[1] - 'A' : 0;
  unsigned int c2 = (sample[2] >= 'A' && sample[2] <= 'Z') ? sample[2] - 'A' : 0;
  return (c0 * 676) + (c1 * 26) + c2;
}

static int load_trigram_freqs(const char *path) {
  // Initialize every entry to the floor value so unseen trigrams
  // don't read uninitialized memory later.
  for (int i = 0; i < NGRAM_SPACE; i++) {
    str_freqs[i] = FLOOR_FREQ;
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    perror("Error opening file");
    return -1;
  }

  char sample_str[5];
  double sample_freq;
  // %lf for a double, and don't forget the & on sample_freq.
  while (fscanf(file, "%4s %lf", sample_str, &sample_freq) == 2) {
    if (sample_freq > 0) {
      str_freqs[ngramhash(sample_str)] = sample_freq;
    }
  }
  fclose(file);
  return 0;
}

double scorengram(const char *text, const int textlen) {
  if (!str_freqs_loaded) {
    if (load_trigram_freqs("english_trigrams.txt") != 0) {
      return 0;
    }
    str_freqs_loaded = 1;
  }

  // Determine the usable length up to a newline (or textlen), same
  // semantics as the original break-on-'\n' behavior.
  int len = textlen;
  for (int i = 0; i < textlen; i++) {
    if (text[i] == '\n') {
      len = i;
      break;
    }
  }

  double sigma = 0;
  // Sliding window over every overlapping trigram: i, i+1, i+2.
  for (int i = 0; i + 2 < len; i++) {
    unsigned int hash = ngramhash(&text[i]);
    double p = str_freqs[hash];
    sigma += log(p);
  }

  return sigma;
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
    plugs[i].score = 0;
  }

  const int steckerlen = sizeof(ISTECKER) / sizeof(ISTECKER[0]);
  const int textlen = strlen(ciphertext); // doesn't change across the loop

  for (int i = 0; i < steckerlen; i++) {
    char *temptext = malloc(textlen + 1);
    strcpy(temptext, ciphertext);

    Plugboard pb = {0};
    parse_plugboard(ISTECKER[i], &pb);
    encrypt_plugboard(&pb, temptext);
    double pairscore = scorengram(temptext, textlen);
    free(temptext);

    // Find where this candidate belongs in the sorted (best-first) top-20.
    // A slot is "empty" if both letters are '\0'. We insert at the first
    // position that is either empty or worse than the new candidate, then
    // shift everything after it down by one, dropping the last entry.
    int insert_idx = -1;
    for (int j = 0; j < 20; j++) {
      int slot_empty = (plugs[j].plug.a == '\0' && plugs[j].plug.b == '\0');
      if (slot_empty || pairscore > plugs[j].score) {
        insert_idx = j;
        break;
      }
    }

    if (insert_idx != -1) {
      for (int k = 19; k > insert_idx; k--) {
        plugs[k] = plugs[k - 1];
      }
      plugs[insert_idx].plug.a = ISTECKER[i][0];
      plugs[insert_idx].plug.b = ISTECKER[i][1];
      plugs[insert_idx].score = pairscore;
    }
  }
  printplugs(plugs, 20);
}
