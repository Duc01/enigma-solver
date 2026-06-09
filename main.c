#include "plugboard.h"

#include <stdio.h>

void readplgbrd(char *plgbrdconf, char *input, char *outbuffer);

// IMPOSE A INPUT CHARACTER LIMIT FOR INPUT TEXT AS strlen() returns type size_t
// and size_t -> int type conversion is unsafe above 32bit integer limit
int main(int argc, char **argv) {
  if (argc == 1 || argc >= 3) {
    fprintf(stderr, "Usage: [%s] filename.txt", argv[0]);
    return 1;
  }

  Plugboard pb = {0};
  PlugboardError err = parse_plugboard("AB CD EF GH IJ KL MN OP QR ST", &pb);
  if (err == PLUGBOARD_OK)
    print_plugboard(&pb);

  char input[] = "HELLO THERE";
  encrypt_plugboard(&pb, input);

  printf("%s\n", input);
  return 0;
}
