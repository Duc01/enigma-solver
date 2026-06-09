#ifndef PLUGBOARD_H
#define PLUGBOARD_H

#define TOTAL_PAIRS 10
#define PAIR_SIZE 2
#define PLUGBOARD_LEN TOTAL_PAIRS *PAIR_SIZE // 20

typedef struct {
  char a;
  char b;
} Pair;

typedef struct {
  Pair pairs[TOTAL_PAIRS];
  char joint[PLUGBOARD_LEN + 1];
  char wiredchars[26];
} Plugboard;

typedef enum {
  PLUGBOARD_OK = 0,           /* success                                    */
  PLUGBOARD_ERR_NULL = -1,    /* NULL pointer argument                      */
  PLUGBOARD_ERR_FORMAT = -2,  /* too few / malformed pair tokens             */
  PLUGBOARD_ERR_INVALID = -3, /* non-uppercase character inside a pair      */
  PLUGBOARD_ERR_DUPE = -4,    /* letter appears in more than one pair       */
} PlugboardError;

PlugboardError parse_plugboard(const char *input, Plugboard *pb);

void print_plugboard(Plugboard *pb);

void encrypt_plugboard(Plugboard *pb, char *input);

#endif /* PLUGBOARD_H */
