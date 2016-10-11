// howStuck.cxx

#include "howStuck.h"

int howStuck(int adc) {
  if ( 64*(adc/64) == adc ) return 1;
  int adcn = adc + 1;
  if ( 64*(adcn/64) == adcn ) return 2;
  return 0;
}
