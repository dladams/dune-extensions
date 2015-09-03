// test_TpcTypes.cxx

// David Adams
// May 2015
//
// Test script for Range.

#include "DXUtil/TpcTypes.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;

int main() {
  const string myname = "test_TpcTypes: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  tpc::Channel c = 1;
  tpc::Index i1 = 2;
  tpc::Tick t1 = -3;
  assert(c == 1);
  assert(i1 == 2);
  assert(t1 == -3);
  tpc::IndexVector is;
  is.push_back(11);
  is.push_back(12);
  assert(is.size() == 2 );
  cout << myname << "Ending test" << endl;
  return 0;
}
