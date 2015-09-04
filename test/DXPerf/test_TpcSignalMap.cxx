// test_TpcSignalMap.cxx

// David Adams
// May 2015
//
// Test script for TpcSignalMap.

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
  int dbg = 0;

  cout << myname << line << endl;
  cout << myname << "Default signal map:" << endl;
  TpcSignalMap sm0;
  sm0.print(cout);
  assert(sm0.check() == 0);
  assert(sm0.channelCount() == 0);
  assert(sm0.tickCount() == 0);
  assert(sm0.binCount() == 0);

  cout << myname << line << endl;
  cout << myname << "Create and check geometry:" << endl;
  GeoHelper gh("dune35t4apa_v5", true);
  if ( dbg ) gh.print();
  assert(gh.geometry() != nullptr);

  cout << myname << line << endl;
  cout << myname << "Local fill of signal map:" << endl;
  TpcSignalMap sm1("sm1", &gh, false);
  sm1.setDbg(dbg);
  assert(sm1.check() == 0);
  assert( sm1.addSignal(1500, 201, 4.1) == 0);
  assert( sm1.addSignal(1500, 202, 8.1) == 0);
  assert( sm1.addSignal(1500, 204, 7.1) == 0);
  assert( sm1.addSignal(1500, 205, 7.1) == 0);
  assert( sm1.addSignal(1501, 204, 2.1) == 0);
  assert( sm1.addSignal(1501, 205, 6.1) == 0);
  assert( sm1.addSignal(1501, 206, 9.1) == 0);
  assert( sm1.addSignal(1501, 207, 3.1) == 0);
  sm1.print(cout, 3, myname);
  cout << myname << "Channel count: " << sm1.channelCount() << endl;
  cout << myname << "Tick count: " << sm1.tickCount() << endl;
  cout << myname << "Bin count: " << sm1.binCount() << endl;
  assert(sm1.channelCount() == 2);
  assert(sm1.tickCount() == 7);
  assert(sm1.binCount() == 8);

  cout << myname << line << endl;
  cout << myname << "Ending test" << endl;
  return 0;
}
