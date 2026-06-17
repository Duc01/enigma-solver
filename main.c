#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "plugboard.h"
#include "rotors.h"

void getsettings(char *filepath, Plugboard *pb) {
  FILE *file = fopen(filepath, "r");
  if (file != NULL) {
    char buf;
    char plugboardinput[29];
    int i = 0;
    while (fread(&buf, sizeof(char), 1, file)) {
      if (!((buf >= 'A' && buf <= 'Z') || buf == ' ' || buf == '\r' ||
            buf == '\n')) {
        fprintf(
            stderr,
            "Plugboard at %s must contain only uppercase english characters\n",
            filepath);
        fclose(file);
        return;
      } else {
        plugboardinput[i] = buf;
        i++;
      }
    }
    plugboardinput[i] = '\0';

    printf("%s\n", plugboardinput);
    parse_plugboard(plugboardinput, pb);
    fclose(file);
  } else {
    perror("Incorrect path");
  }
}

// IMPOSE A INPUT CHARACTER LIMIT FOR INPUT TEXT AS strlen() returns type size_t
// and size_t -> int type conversion is unsafe above 32bit integer limit
int main(int argc, char **argv) {
  if (argc == 1 || argc >= 3) {
    fprintf(stderr, "Usage: [%s] filename.txt", argv[0]);
    return 1;
  }
  // TODO: Impose limitation of only uppercase characters
  Plugboard *pb = &(Plugboard){0};
  getsettings(argv[1], pb);
  printf("%s\n", pb->wiredchars);

  // int rotorsetup[] = {0, 1, 2};
  // int rotoroffsets[] = {0, 0, 0};
  // int ringoffsets[] = {0, 0, 0};
  // int rotorcount = 3;
  return 0;
}
