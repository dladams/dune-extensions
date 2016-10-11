// howStuck.h

// Returns if and how bits are stuck for 35-ton data.
//   0 - not stuck
//   1 - lower six bits are unset (0)
//   2 - lower six bits are set (1)

int howStuck(int adc);
