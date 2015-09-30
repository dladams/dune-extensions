// test_SimChannelTupler.cxx

// David Adams
// May 2015
//
// Test script for SimChannelTupler.

#include "DXPerf/SimChannelTupler.h"
#include "AXService/ArtServiceHelper.h"
#include "TFile.h"
#include "TTree.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Geometry/Geometry.h"
#include "Utilities/DetectorProperties.h"
#include "Utilities/LArProperties.h"
#include "Simulation/SimChannel.h"
#include "DXGeometry/GeoHelper.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using sim::SimChannel;

typedef SimChannelTupler::SimChannelVector SimChannelVector;

//**********************************************************************

// Add ionization to the relevant sim channels.
void addIonization(SimChannelVector& scs, int trackid, const double postim[4],
                   double qdep, double edep, GeoHelper* pgeo) {
  PlanePositionVector pps = pgeo->planePositions(postim);
  for ( PlanePosition pp : pps ) {
    SimChannel* psc = nullptr;
    // Look for existing SC for this channel.
    for ( SimChannel& sc : scs ) {
      if ( sc.Channel() == pp.channel ) {
        psc = &sc;
        break;
      }
    }
    // If not found, create one.
    if ( psc == nullptr ) {
      scs.push_back(SimChannel(pp.channel));
      psc = &scs.back();
    }
    // Add current ionization to the sim channel.
    double postim2[4] = {postim[0], postim[1], postim[2], postim[3]};
    psc->AddIonizationElectrons(trackid, pp.planeid.TPC, qdep, postim2, edep);
  }
}

// Return points along a track.
const double* trackpos() {
  static double postim[4] = {100.0, 70.0, 30.0, 0.0};
  postim[0] +=  1.0;
  postim[1] += -0.5;
  postim[2] +=  0.4;
  return postim;
}

//**********************************************************************

int main() {
  const string myname = "test_SimChannelTupler: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  // Default map.
  string line = "-----------------------------";
  int dbg = 1;

  // Geometry name.
  string gname = "dune35t4apa_v5";

  cout << line << endl;
  cout << myname << "Add TFileService." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  string scfg = "fileName: \"simchannel_test.root\" service_type: \"TFileService\"";
  assert( ash.addService("TFileService", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the TimeService service (needed for DetectorProperties)." << endl;
  scfg = "ClockSpeedExternal: 3.125e1 ClockSpeedOptical: 128 ClockSpeedTPC: 2 ClockSpeedTrigger: 16 DefaultBeamTime: 0 DefaultTrigTime: 0 FramePeriod: 1600 G4RefTime: 0 InheritClockConfig: false TrigModuleName: \"\" TriggerOffsetTPC: 0";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("TimeService", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the DetectorProperties service (used by GeoHelper)." << endl;
  scfg = "ElectronsToADC: 6.8906513e-3 NumberTimeSamples: 3200 ReadOutWindowSize: 3200 TimeOffsetU: 0 TimeOffsetV: 0 TimeOffsetZ: 0";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("DetectorProperties", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the LArProperties service (used by DetectorProperties)." << endl;
  scfg = "prodsingle_dune35t.fcl";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("LArProperties", scfg, true) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the DatabaseUtil service (needed for LArProperties)." << endl;
  scfg = "DBHostName: \"fnalpgsdev.fnal.gov\" DBName: \"dune_dev\" DBUser: \"dune_reader\" PassFileName: \".lpswd\" Port: 5438 ShouldConnect: false TableName: \"main_run\" ToughErrorTreatment: false";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("DatabaseUtil", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the Geometry service (used by DetectorProperties)." << endl;
  scfg = "DisableWiresInG4: true GDML: \"dune35t4apa_v5.gdml\" Name: \"" + gname +
         "\" ROOT: \"" + gname + "\" SortingParameters: { DetectorVersion: \"" + gname +
         "\" } SurfaceY: 0";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("Geometry", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the DUNE geometry helper service (required to load DUNE geometry)." << endl;
  scfg = "service_provider: \"DUNEGeometryHelper\"";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("ExptGeoHelperInterface", scfg) == 0 );

  cout << line << endl;
  cout << myname << "Load Services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Check services." << endl;
  art::ServiceHandle<util::DetectorProperties> hdp;
  art::ServiceHandle<util::LArProperties> hlp;
  art::ServiceHandle<geo::Geometry> hgeo;
  assert( hdp.operator->() != nullptr );
  assert( hlp.operator->() != nullptr );
  assert( hgeo.operator->() != nullptr );

  cout << myname << line << endl;
  cout << myname << "Create and check geometry:" << endl;
  GeoHelper gh(hgeo.operator->(), true);
  //GeoHelper gh(gname", true);
  if ( dbg ) gh.print();
  assert(gh.geometry() != nullptr);
  assert(gh.detectorProperties() != nullptr );

  cout << myname << line << endl;
  cout << myname << "Fetch TFile service." << endl;
  art::ServiceHandle<art::TFileService> hfs;
  art::TFileService* pfs = &*hfs;
  assert( pfs != nullptr );
  assert( pfs->file().IsOpen() );

  cout << myname << line << endl;
  cout << myname << "Create tree." << endl;
  SimChannelTupler sct(gh, *hfs, 10000);
  assert( sct.tree() != nullptr );
  sct.tree()->Print();
  assert( sct.tree()->GetEntries() == 0 );
  assert( sct.tree()->GetName() == string("simchan") );

  cout << myname << line << endl;
  cout << myname << "Create event ID." << endl;
  art::EventID evid(101, 0, 1001);
  cout << "    Run: " << evid.run() << endl;
  cout << "  Event: " << evid.event() << endl;

  cout << myname << line << endl;
  cout << myname << "Create some sim channels." << endl;
  SimChannelTupler::SimChannelVector scs;
  addIonization(scs, 1, trackpos(), 10000, 0.20, &gh);
  addIonization(scs, 1, trackpos(), 12000, 0.25, &gh);
  addIonization(scs, 1, trackpos(), 13000, 0.30, &gh);
  addIonization(scs, 1, trackpos(), 11000, 0.22, &gh);
  addIonization(scs, 1, trackpos(), 12000, 0.24, &gh);
  cout << myname << "SimChannel count: " << scs.size() << endl;

  cout << myname << line << endl;
  cout << myname << "Fill tree with sim channels." << endl;
  sct.fill(evid, scs);
  sct.tree()->Scan("run:event:nchan:chan:energy:charge");
  assert( sct.tree()->GetEntries() > 0 );

  cout << myname << line << endl;
  cout << myname << "Close services." << endl;
  ArtServiceHelper::close();

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}

//**********************************************************************
