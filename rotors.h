#ifndef ROTORS_H
#define ROTORS_H

void increment_rotors(int rotoroffsets[], int rotorcount);

void runthrough(int rotoroffset, char *input, bool forward, int activerotor);
void iteraterotors(int rotoroffsets[], char *input, bool forward,
                   int activerotors[], int rotorcount);
void encode_char(char *input, int rotoroffsets[], int activerotors[],
                 int rotorcount);

void reflector(char *input);

#endif /* ROTORS_H */
