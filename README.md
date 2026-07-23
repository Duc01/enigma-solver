# enigma-solver
This project is meant to serve a command line tool to encrypt and decrypt enigma messages, based on the M3 model of enigma machine. While the project contains some function to break the engima cipher, the techniques are currently very rudimentary and incaccurate.
## Usage:
```bash
./enigma settings.txt input.txt
```
Here somewhat obviously the `settings.txt` contains the original settings for the enigma machine while the `input.txt` file contains the input text (all uppercase with no spaces and in one line).
The settings.txt file has a very specific formatting which looks something like this:
```bash
0 1 2 12 05 01 HR AT IW SK UY DF GV LJ BO MX
```
Here the first three letters are rotors used in each of the positions, they are all zero indexed with the first rotor being the fastest and the last being the slowest. The next three elements are the the offsets for each succesive rotor with a leading zero for historical accuracy followed by the ten plugboard configuraitons.
# Features
- Accurate implementation of M3 model enigma machine
- Proper emulation of both the rotors and plugboard mechanisms
- Ability to encrypt and decrypt enigma machines
# Future ideas
- [ ] Add ability to break the enigma cipher through greedy search algorithms and hill climbing
- [ ] Add more polished CLI interface
- [ ] Improve memory safety (current program hasn't had much thought put in this regard)
