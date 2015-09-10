// test_test_MCTrajectoryFollower.cxx

// David Adams
// September 2015
//
// Test script for test_MCTrajectoryFollower.cxx

#include "DXPerf/MCTrajectoryFollower.h"

#include <string>
#include <iostream>
#include <cassert>
#include "DXGeometry/GeoHelper.h"

using std::string;
using std::cout;
using std::endl;
using simb::MCParticle;

int main() {
  const string myname = "test_MCTrajectoryFollower: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";
  // Create MC particle.
  cout << myname << line << endl;
  cout << myname << "Add a muon." << endl;
  double m = 0.1057;
  double pz = 10.0;
  MCParticle par0(0, 13, "primary", -1, m, 1);
  TLorentzVector pos;
  TLorentzVector mom;
  pos.SetXYZT(0.0, 0.0, 0.0, 0.0);
  mom.SetXYZM(0.1*pz, -0.2*pz, pz, m);
  par0.SetGvtx(pos);
  TLorentzVector dpos(1.0, -2.0, 10.0, 0.35);
  for ( int ipt=0; ipt<10; ++ipt) {
    pos += dpos;
    par0.AddTrajectoryPoint(pos, mom);
    pos.Print();
  }
  // Create follower.
  cout << myname << line << endl;
  cout << myname << "Create geometry helper." << endl;
  GeoHelper gh("dune35t4apa_v5");
  // Create follower.
  cout << myname << line << endl;
  cout << myname << "Create follower." << endl;
  MCTrajectoryFollower f1(0.1, "f1", &gh, 0, 4);
  f1.addMCParticle(par0);
  cout << myname << line << endl;
  cout << myname << "Ending test" << endl;
  return 0;
}
