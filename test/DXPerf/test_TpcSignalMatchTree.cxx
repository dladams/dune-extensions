// test_TpcSignalMatchTree.cxx

// David Adams
// May 2015
//
// Test script for TpcSignalMatchTree.

#include "DXPerf/TpcSignalMatchTree.h"
#include "DXPerf/TpcSignalMap.h"
#include "DXPerf/TpcSignalMatcher.h"
#include "dune/ArtSupport/ArtServiceHelper.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "TTree.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using tpc::Index;
using tpc::badIndex;
typedef TpcSignalMap::TpcSignalMapVector TpcSignalMapVector;

int main() {
  const string myname = "test_TpcSignalMatchTree: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  // Default map.
  string line = "-----------------------------";
  int dbg = 0;

  cout << line << endl;
  cout << myname << "Load TFileService." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  string scfg = "fileName: \"tpctest.root\" service_type: \"TFileService\"";
  assert( ash.addService("TFileService", scfg) == 0 );
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Create and check geometry:" << endl;
  GeoHelper gh("dune35t4apa_v5", true);
  if ( dbg ) gh.print();
  assert(gh.geometry() != nullptr);

  cout << myname << line << endl;
  cout << myname << "Local fill of signal map 1:" << endl;
  TpcSignalMap sm1("sm1", &gh, false);
  Index myrop = 11;
  assert(sm1.check() == 0);
  assert(sm1.usetpc() == false);
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
  cout << myname << "Split signal map 1:" << endl;
  TpcSignalMapVector sms1;
  assert( sm1.splitByRop(sms1) == 0 );
  cout << myname << "Split vector size: " << sms1.size() << endl;
  assert( sms1.size() == 2 );
  assert( sms1[1]->rop() == myrop );

  cout << myname << line << endl;
  cout << myname << "Split signal map 2:" << endl;
  TpcSignalMapVector sms2;
  assert( sm2.splitByRop(sms2) == 0 );
  cout << myname << "Split vector size: " << sms2.size() << endl;
  assert( sms2.size() == 1 );
  assert( sms2[0]->rop() == myrop );

  cout << myname << line << endl;
  cout << myname << "Create match." << endl;
  TpcSignalMatcher mat(sms1, sms2, true);
  cout << "Match size: " << mat.size() << endl;
  assert( &mat.referenceVector() == &sms1 );
  assert( &mat.matchVector() == &sms2 );
  assert( mat.size() == sms1.size() );

  cout << myname << line << endl;
  cout << myname << "Print match with option 0:" << endl;
  mat.print(cout, 0);

  cout << myname << line << endl;
  cout << myname << "Create tree." << endl;
  TpcSignalMatchTree mt("match");
  assert( mt.tree() != nullptr );
  mt.tree()->Print();
  assert( mt.tree()->GetEntries() == 0 );
  assert( mt.tree()->GetName() == string("match") );

  cout << myname << line << endl;
  cout << myname << "Create event ID." << endl;
  art::EventID evid(101, 0, 1001);
  cout << "    Run: " << evid.run() << endl;
  cout << "  Event: " << evid.event() << endl;

  cout << myname << line << endl;
  cout << myname << "Fill tree with matches." << endl;
  mt.fill(evid, mat);
  mt.tree()->Scan("run:event:stat:ref:match:distance:rop");
  assert( mt.tree()->GetEntries() == 2 );

  cout << myname << line << endl;
  cout << myname << "Close services." << endl;
  ArtServiceHelper::close();

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}
