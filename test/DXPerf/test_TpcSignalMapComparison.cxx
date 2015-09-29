// test_TpcSignalMapComparison.cxx

// David Adams
// May 2015
//
// Test script for TpcSignalMapComparison.

#include "DXPerf/TpcSignalMapComparison.h"
#include "DXPerf/TpcSignalMap.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using tpc::Index;
using tpc::badIndex;

int main() {
  const string myname = "test_TpcSignalMapComparison: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  // Default map.
  string line = "-----------------------------";
  int dbg = 0;

  cout << myname << line << endl;
  cout << myname << "Create and check geometry:" << endl;
  GeoHelper gh("dune35t4apa_v5", true);
  if ( dbg ) gh.print();
  assert(gh.geometry() != nullptr);

  cout << myname << line << endl;
  cout << myname << "Local fill of signal map 1:" << endl;
  TpcSignalMap sm1("sm1", &gh, false);
  Index myrop = 11;
  sm1.setRop(myrop);
  assert(sm1.check() == 0);
  assert( sm1.addSignal( 120, 200, 1.1) == 0);
  assert( sm1.addSignal(1498, 200, 9.1) == 0);
  assert( sm1.addSignal(1499, 200, 6.1) == 0);
  assert( sm1.addSignal(1500, 201, 4.1) == 0);
  assert( sm1.addSignal(1500, 202, 8.1) == 0);
  assert( sm1.addSignal(1500, 204, 7.1) == 0);
  assert( sm1.addSignal(1500, 205, 7.1) == 0);
  assert( sm1.addSignal(1501, 204, 2.1) == 0);
  assert( sm1.addSignal(1501, 205, 6.1) == 0);
  assert( sm1.addSignal(1501, 206, 9.1) == 0);
  assert( sm1.addSignal(1501, 207, 3.1) == 0);
  sm1.print(cout, 3, myname);

  cout << myname << line << endl;
  cout << myname << "Local fill of signal map 2:" << endl;
  TpcSignalMap sm2("sm2", &gh, false);
  sm2.setRop(myrop);
  assert(sm2.check() == 0);
  assert( sm2.addSignal(1500, 201, 4.1) == 0);
  assert( sm2.addSignal(1500, 202, 8.1) == 0);
  assert( sm2.addSignal(1500, 203, 3.1) == 0);
  assert( sm2.addSignal(1500, 204, 7.1) == 0);
  assert( sm2.addSignal(1500, 205, 7.1) == 0);
  assert( sm2.addSignal(1501, 204, 2.1) == 0);
  assert( sm2.addSignal(1501, 205, 6.1) == 0);
  assert( sm2.addSignal(1501, 206, 9.1) == 0);
  assert( sm2.addSignal(1501, 207, 3.1) == 0);
  assert( sm2.addSignal(1502, 208, 5.1) == 0);
  assert( sm2.addSignal(1503, 209, 4.1) == 0);
  assert( sm2.addSignal(1504, 210, 8.1) == 0);
  sm2.print(cout, 3, myname);

  cout << myname << line << endl;
  cout << myname << "Create comparison:" << endl;
  TpcSignalMapComparison cmp(sm1, sm2);
  cmp.print(cout, myname);

  cout << myname << line << endl;
  cout << myname << "Check comparison." << endl;
  assert( &cmp.reference() == &sm1 );
  assert( &cmp.match() == &sm2 );
  assert( cmp.rop() == myrop );
  assert( cmp.referenceChannelCount() == 4 );
  assert( cmp.matchChannelCount() == 5 );
  assert( cmp.channelFraction() == 0.5 );
  assert( cmp.binFraction() == 0.8 );

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}
