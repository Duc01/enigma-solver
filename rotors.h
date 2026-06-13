#ifndef ROTORS_H
#define ROTORS_H

void encrypt_str(int rotorconf[], int rotorcount, char *input);
void encrypt_rotors(int rotorconf[], int rotorcount, char *input);

void increment_rotor(int rotorconf[], int rotorcount);
void increment_rotors(int rotoroffsets[], int rotorcount);

char runthrough(int rotoroffset, char input, bool forward, int activerotor);
char iteraterotors(int rotoroffsets[], char input, bool forward,
                   int activerotors[], int rotorcount);
char encode_char(char input, int rotoroffsets[], int activerotors[],
                 int rotorcount);
#endif /* ROTORS_H */
