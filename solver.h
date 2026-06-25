#ifndef SOLVER_H
#define SOLVER_H

typedef struct {
  int rotor[3];
  int offsets[3];
  int rings[3];
  double score;
} rotorsetup;

int frequency(char target, const char *text);
// double indexofcoincidence(char *text);

/**
 * @brief tries to find top five most likely rotor configurations
 * @param[in] rotorsetup The array into which rotors are saved
 * @param[in] ciphertext The ciphertext to test against
 */
void fixrotors(rotorsetup possiblerotors[5], const char *ciphertext);

// void fixrotors(int rotorsetup[3], char *ciphertext);

#endif // SOLVER_H
