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
#include "DXArt/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "TFile.h"
#include "TTree.h"

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

  // Provide access to art services.
  cout << line << endl;
  cout << myname << "Retrieve service helper.";
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  cout << myname << "Add TFileService" << endl;
  string scfg = "TFileService: { fileName: \"mctraj_test.root\" service_type: \"TFileService\"}";
  assert( ash.addService("TFileService", scfg) == 0 );
  cout << myname << "Load services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << "Add a muon." << endl;
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

  // Fetch geometry helper.
  // We could but don't use geometry service for this.
  cout << myname << line << endl;
  cout << myname << "Create geometry helper." << endl;
  GeoHelper gh("dune35t4apa_v5");

  // Create follower.
  cout << myname << line << endl;
  cout << myname << "Create follower." << endl;
  MCTrajectoryFollower f1(0.1, "f1", &gh, 0, 4);

  cout << myname << line << endl;
  cout << myname << "Check file and existence of follower tree." << endl;
  art::ServiceHandle<art::TFileService> pfs;
  TObject* pobj = pfs->file().Get("f1");
  TTree* ptree = dynamic_cast<TTree*>(pobj);
  assert( pobj != nullptr );
  assert( ptree != nullptr );
  cout << "Get TFile service." << endl;
  cout << "Check if TFile is open." << endl;
  assert( pfs->file().IsOpen() );

  cout << myname << line << endl;
  cout << myname << "Add particle." << endl;
  f1.addMCParticle(par0);
  cout << myname << "Tree entry count: " << ptree->GetEntries() << endl;

  cout << myname << line << endl;
  pfs->file().ls();
  ArtServiceHelper::close();
  cout << myname << "Ending test" << endl;
  return 0;
}
