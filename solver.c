#include <float.h>
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
              int initial_offsets[3] = {a, b, c};

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
                  possiblerotors[idx].offsets[0] =
                      (initial_offsets[0] + 1) % 26;
                  possiblerotors[idx].offsets[1] =
                      (initial_offsets[1] + 1) % 26;
                  possiblerotors[idx].offsets[2] =
                      (initial_offsets[2] + 1) % 26;
                  break;
                }
              }
            }
          }
        }
        free(input);
      }
    }
  }
  // flipping the offsets so they're compatible with other functions
  //  all function expect a format where the left most rotor in the array is the
  //  rightmost (or first) motor in the system
  int old[3];
  for (int i = 0; i < 5; i++) {
    // Reverse offsets
    memcpy(old, possiblerotors[i].offsets, sizeof(possiblerotors[i].offsets));
    for (int j = 0; j < 3; j++) {
      possiblerotors[i].offsets[j] = old[3 - j - 1];
    }

    // Reverse rotors
    memcpy(old, possiblerotors[i].rotor, sizeof(possiblerotors[i].rotor));
    for (int j = 0; j < 3; j++) {
      possiblerotors[i].rotor[j] = old[3 - j - 1];
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
  unsigned int c0 =
      (sample[0] >= 'A' && sample[0] <= 'Z') ? sample[0] - 'A' : 0;
  unsigned int c1 =
      (sample[1] >= 'A' && sample[1] <= 'Z') ? sample[1] - 'A' : 0;
  unsigned int c2 =
      (sample[2] >= 'A' && sample[2] <= 'Z') ? sample[2] - 'A' : 0;
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
    if (text[i] >= 'A' && text[i] <= 'Z' && text[i + 1] >= 'A' &&
        text[i + 1] <= 'Z' && text[i + 2] >= 'A' && text[i + 2] <= 'Z') {
      unsigned int hash = ngramhash(&text[i]);
      double p = str_freqs[hash];
      sigma += log(p);
    }
  }

  return sigma;
}

void fixplugs(PlugSetup plugs[20], const char *ciphertext,
              RotorSetup *best_rotor) {
  for (int i = 0; i < 20; i++) {
    plugs[i].plug.a = '\0';
    plugs[i].plug.b = '\0';
    plugs[i].score = DBL_MIN;
  }

  const int textlen = strlen(ciphertext);
  Plugboard current_pb;
  parse_plugboard("", &current_pb);

  char found_plugs[27] = {0};
  int found_count = 0;

  for (int p = 0; p < 10; p++) {
    char best_a = '\0';
    char best_b = '\0';
    double best_score = -1e12;

    for (char i = 'A'; i <= 'Z'; i++) {
      if (strchr(found_plugs, i))
        continue;

      for (char j = i + 1; j <= 'Z'; j++) {
        if (strchr(found_plugs, j))
          continue;

        Plugboard test_pb = current_pb;
        test_pb.wiredchars[i - 'A'] = j;
        test_pb.wiredchars[j - 'A'] = i;

        char *temptext = malloc(textlen + 1);
        if (temptext == NULL)
          continue;
        strcpy(temptext, ciphertext);

        // Full decryption: pb * rotors * pb * ciphertext
        encrypt_str(temptext, best_rotor->offsets, best_rotor->rings,
                    best_rotor->rotor, 3, &test_pb);

        double score = scorengram(temptext, textlen);
        free(temptext);

        if (score > best_score) {
          best_score = score;
          best_a = i;
          best_b = j;
        }
      }
    }

    if (best_a != '\0' && best_b != '\0') {
      current_pb.wiredchars[best_a - 'A'] = best_b;
      current_pb.wiredchars[best_b - 'A'] = best_a;
      found_plugs[found_count++] = best_a;
      found_plugs[found_count++] = best_b;
      found_plugs[found_count] = '\0';

      plugs[p].plug.a = best_a;
      plugs[p].plug.b = best_b;
      plugs[p].score = best_score;
    } else {
      break;
    }
  }

  printplugs(plugs, 20);

  char *final_decrypted = malloc(textlen + 1);
  if (final_decrypted != NULL) {
    strcpy(final_decrypted, ciphertext);
    encrypt_str(final_decrypted, best_rotor->offsets, best_rotor->rings,
                best_rotor->rotor, 3, &current_pb);
    printf("\nDecrypted Plaintext:\n%s\n\n", final_decrypted);
    free(final_decrypted);
  }
}
