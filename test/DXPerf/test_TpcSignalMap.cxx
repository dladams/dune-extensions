// test_TpcSignalMap.cxx

// David Adams
// May 2015
//
// Test script for TpcSignalMap.

#include "DXPerf/TpcSignalMap.h"

#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::istringstream;
using tpc::badIndex;

typedef TpcSignalMap::TpcSignalMapPtr TpcSignalMapPtr;
typedef TpcSignalMap::TpcSignalMapVector TpcSignalMapVector;

int main(int argc, char* argv[]) {
  const string myname = "test_TpcSignalMap: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  // Default map.
  string line = "-----------------------------";

  int dbg = 0;
  if ( argc > 1 ) {
    istringstream ssdbg(argv[1]);
    ssdbg >> dbg;
  }

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
  TpcSignalMap sm1("sm1", &gh, true);
  sm1.setDbg(dbg);
  assert(sm1.check() == 0);
  assert( sm1.addSignal(1200, 201, 4.1, 1) != 0);
  cout << myname << line << endl;
  assert( sm1.addSignal(1200, 201, 4.1, 4) == 0);
  assert( sm1.addSignal(1200, 202, 8.1, 4) == 0);
  assert( sm1.addSignal(1200, 204, 7.1, 4) == 0);
  assert( sm1.addSignal(1200, 205, 3.1, 4) == 0);
  assert( sm1.addSignal(1201, 205, 1.1, 4) == 0);
  assert( sm1.addSignal(1201, 206, 5.1, 4) == 0);
  assert( sm1.addSignal(1201, 207, 2.1, 4) == 0);
  assert( sm1.addSignal(1201, 204, 2.1, 5) == 0);
  assert( sm1.addSignal(1201, 205, 6.1, 5) == 0);
  assert( sm1.addSignal(1201, 206, 9.1, 5) == 0);
  assert( sm1.addSignal(1201, 207, 3.1, 5) == 0);
  assert( sm1.addSignal(1201, 208, 0.9, 5) == 0);
  sm1.print(cout, 3, myname);
  cout << myname << "Channel count: " << sm1.channelCount() << endl;
  cout << myname << "Tick count: " << sm1.tickCount() << endl;
  cout << myname << "Bin count: " << sm1.binCount() << endl;
  cout << myname << "ROP (readout plane): " << sm1.rop() << endl;
  assert(sm1.channelCount() == 2);
  assert(sm1.tickCount() == 8);
  assert(sm1.binCount() == 12);
  assert(sm1.rop() == badIndex());

  cout << myname << "Split signal map." << endl;
  TpcSignalMapVector sms;
  sm1.splitByRop(sms, true);
  cout << myname << "# split maps: " << sms.size() << endl;
  cout << myname << line << endl;
  for ( const TpcSignalMapPtr psm : sms ) {
    psm->print(cout);
    cout << myname << line << endl;
  }
  cout << myname << "Split signal map checks." << endl;
  assert(sms.size() == 2);
  assert(sms[0]->rop() == 9);
  assert(sms[1]->rop() == 9);
  assert(sms[0]->channelCount() == 2);
  assert(sms[1]->channelCount() == 1);
  assert(sms[0]->tickCount() == 7);
  assert(sms[1]->tickCount() == 5);

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}
