// test_reducedPDG.cxx

// David Adams
// September 2015
//
// Test script for Range.

#include "DXUtil/reducedPDG.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;

int main() {
  const string myname = "test_reducedPDG: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  assert( reducedPDG(11) == 1 );
  assert( reducedPDG(-11) == -1 );
  assert( reducedPDG(13) == 2 );
  assert( reducedPDG(-2212) == -3 );
  assert( reducedPDG(99) == -12 );
  cout << myname << "Done." << endl;
  return 0;
}
