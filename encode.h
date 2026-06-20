#ifndef ENCODE_H
#define ENCODE_H
#include "plugboard.h"
void encrypt_str(char *input, int rotoroffsets[], int ringoffsets[],
                 int activerotors[], int rotorcount, Plugboard *pb);
#endif // ENCODE_H
