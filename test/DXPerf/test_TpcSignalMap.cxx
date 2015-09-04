// test_TpcSignalMap.cxx

// David Adams
// May 2015
//
// Test script for Range.

#include "DXPerf/TpcSignalMap.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;

int main() {
  const string myname = "test_TpcSignalMap: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  // Default map.
  string line = "-----------------------------";

  cout << myname << line << endl;
  cout << myname << "Default signal map:" << endl;
  TpcSignalMap sm0;
  sm0.print(cout);
  assert(sm0.check() == 0);
  assert(sm0.channelCount() == 0);
  assert(sm0.tickCount() == 0);
  assert(sm0.binCount() == 0);

  cout << myname << line << endl;
  cout << myname << "Local fill of signal map:" << endl;
  GeoHelper gh("dune35t4apa_v4", true);
  gh.print();
  assert(gh.geometry() != nullptr);
  TpcSignalMap sm1("sm1", &gh, false);
  assert(sm1.check() == 0);
  assert( sm1.addSignal(11, 101, 41.0) == 0);
  sm1.print(cout, 3, myname);
  assert(sm1.channelCount() == 1);
  assert(sm1.tickCount() == 1);
  assert(sm1.binCount() == 1);

  cout << myname << line << endl;
  cout << myname << "Ending test" << endl;
  return 0;
}
