#ifndef SOLVER_H
#define SOLVER_H

#include "plugboard.h"
typedef struct {
  int rotor[3];
  int offsets[3];
  int rings[3];
  double score;
} RotorSetup;

typedef struct {
  Pair plug;
  double score;
} PlugSetup;

/**
 * @brief tries to find top five most likely rotor configurations
 * @param[in] rotorsetup The array into which rotors are saved
 * @param[in] ciphertext The ciphertext to test against
 */
void fixrotors(RotorSetup possiblerotors[5], const char *ciphertext);

void fixplugs(PlugSetup plugs[20], const char *ciphertext, RotorSetup *best_rotor);

#endif // SOLVER_H
