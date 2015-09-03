// test_intProcess.cxx

// David Adams
// May 2015
//
// Test script for Range.

#include "DXUtil/intProcess.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;

int main() {
  const string myname = "test_intProcess: ";
  cout << myname << "Starting test" << endl;
  assert( intProcess("primary") == 0 );
  assert( intProcess("nCapture") == 5 );
  assert( intProcess("bad") == -1 );
  cout << myname << "Ending test" << endl;
  return 0;
}
