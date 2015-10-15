// test_geometry.cxx

// David Adams
// August 2015
//
// Test script for DUNE geometries.

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "Geometry/GeometryCore.h"
#include "DXGeometry/GeoHelper.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <memory>

using std::string;
using std::cout;
using std::endl;
using std::ostringstream;
using std::istringstream;
using std::vector;
using std::shared_ptr;
using geo::TPCID;
using geo::TPCGeo;

string spos(double pos[3]) {
  ostringstream ssout;
  ssout << "(" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")";
  return ssout.str();
}

bool inrange(double x, double x1, double x2) {
  if ( x2 > x1 ) {
    if ( x < x1 ) return false;
    if ( x > x2 ) return false;
  } else {
    if ( x < x2 ) return false;
    if ( x > x1 ) return false;
  }
  return true;
}

int main(int narg, char** argv) {
  //string geopath = "services.Geometry";
  const string myname = "test_geometry: ";
  vector<string> gnames = {"lbne10kt", "lbne35t", "dune35t4apa_v4", "dune10kt_v1", "dune35t4apa_v5", "dune10kt_v1_workspace"};
  string gname = gnames[3];
  bool useChan = true;
  if ( narg > 1 ) {
    gname = argv[1];
  }
  if ( gname == "-h" || gname == "help" ) {
    cout << "Usage: " << argv[0] << " [GEONAME =dune10kt_v1] [USECHAN =true] [PRINT =0] [DBG =0]" << endl;
    cout << "  GEONAME is any of the following:" << endl;
    for ( string name : gnames ) cout << "    " << name << endl;
    cout << "  USECHAN nonzero to load channel map." << endl;
    cout << "  PRINT nonzero to dump geometry to log." << endl;
    cout << "  DBG is the debug level for GeoHelper." << endl;
    return 0;
  }
  if ( narg > 2 ) {
    string sarg = argv[2];
    useChan = sarg=="true" || sarg=="1";
  }
  int print = 0;
  if ( narg > 3 ) {
    istringstream ssarg(argv[3]);
    ssarg >> print;
  }
  int dbg = 0;
  if ( narg > 4 ) {
    istringstream ssarg(argv[4]);
    ssarg >> dbg;
    cout << "XXX: " << argv[4] << " " << dbg << endl;
  }
  cout << myname << "Detector name: " << gname << endl;
  cout << myname << " Use channels: " << useChan << endl;
  cout << myname << "        Print: " << print << endl;
  cout << myname << "        Debug: " << dbg << endl;
  // (xref,yref,zref) is a point inside a TPC
  vector<double> xref = {-500.0, 0.0, 0.0, -500.0, 0.0, 180.0};
  vector<double> yref = {   0.0, 0.0, 0.0,    0.0, 0.0, 300.0};
  vector<double> zref = {   0.0, 0.0, 0.0,    0.0, 0.0, 100.0};
  vector<int> ncryExp = {2, 1, 1, 1, 1, 1};
  vector<int> ntpcExp = {120, 6, 8, 300, 8, 300};
  cout << "Checking geometry name: " << gname << endl;
  unsigned int ndet = gnames.size();
  unsigned int idet = 0;
  for ( ; idet<ndet; ++idet ) {
    if ( gnames[idet] == gname ) break;
  }
  assert( idet != ndet );
  string spar = R"(Name: ")" + gname + R"("
DisableWiresInG4: true
SurfaceY: 0
)";
  cout << myname << "Config: \n" << spar << endl;
  fhicl::ParameterSet parset;
  fhicl::make_ParameterSet(spar, parset);
  // Create the LAr geometry.
  GeoHelper gh(gname, useChan, dbg);
  if ( print ) {
    gh.print();
    return 0;
  }
  const geo::GeometryCore* pgeo = gh.geometry();
  // Test the geometry.
  cout << myname << "Retrieving basic geometry info" << endl;
  int ncry = pgeo->Ncryostats();
  int ntpc = pgeo->NTPC(0);
  string detname = pgeo->DetectorName();
  double ysurf = pgeo->SurfaceY();
  double totmass = pgeo->TotalMass();
  double pos[3] = {xref[idet], yref[idet], zref[idet]};
  TPCID tid = pgeo->FindTPCAtPosition(pos);
  cout << myname << "Fetching # channels." << endl;
  unsigned int nchan = 0;
  if ( gh.haveChannelMap() ) {
    nchan = pgeo->Nchannels();
  }
  cout << myname << "Fetched # channels." << endl;
  cout << myname << "      # channels: " << nchan << endl;
  assert(tid.isValid);
  TPCGeo tgeo = pgeo->TPC(tid);
  cout << myname << "Displaying basic geometry info" << endl;
  cout << myname << "   Geometry name: " << detname << endl;
  cout << myname << "     # cryostats: " << ncry << endl;
  cout << myname << "    # TPC for C0: " << ntpc << endl;
  cout << myname << "       Surface Y: " << ysurf << " cm" << endl;
  cout << myname << "   Detector mass: " << totmass << " kg" << endl;
  cout << myname << "   Reference pos: " << "(" << pos[0] << ", " << pos[1]
                                         << ", " << pos[2] << ")" << endl;
  cout << myname << "   Reference cry: " << tid.Cryostat << endl;
  cout << myname << "   Reference TPC: " << tid.TPC << endl;
  cout << myname << "      # channels: " << nchan << endl;
  cout << myname << "Displaying channel map info" << endl;
  cout << myname << "           # ROP: " << gh.nrop() << endl;
  cout << myname << "           # APA: " << gh.napa() << endl;
  cout << myname << "Checking basic geometry info" << endl;
  assert(detname==gname);
  assert(ncry==ncryExp[idet]);
  assert(ntpc==ntpcExp[idet]);
  cout << myname << "Ended check of basic geometry" << endl;
  // Check TPC positions.
  cout << myname << "Checking TPC position" << endl;
  //double pos[3] = {-303.606, 587.567, 10.9497};
  double locpos2[3] = {tgeo.HalfWidth(), tgeo.HalfHeight(), 0.5*tgeo.Length()};
  double locpos1[3] = {-locpos2[0], -locpos2[1], -locpos2[2]};
  double pos1[3], pos2[3];
  double locpos[3];
  tgeo.LocalToWorld(locpos1, pos1);
  tgeo.LocalToWorld(locpos2, pos2);
  tgeo.WorldToLocal(pos, locpos);
  cout << myname << "World coordinates:" << endl;
  cout << myname << "         point: " << spos(pos) << endl;
  cout << myname << "  TPC corner 1: " << spos(pos1) << endl;
  cout << myname << "  TPC corner 2: " << spos(pos2) << endl;
  cout << myname << "TPC coordinates:" << endl;
  cout << myname << "         point: " << spos(locpos) << endl;
  cout << myname << "  TPC corner 1: " << spos(locpos1) << endl;
  cout << myname << "  TPC corner 2: " << spos(locpos2) << endl;
  assert( inrange(locpos[0], locpos1[0], locpos2[0]) );
  assert( inrange(locpos[1], locpos1[1], locpos2[1]) );
  assert( inrange(locpos[2], locpos1[2], locpos2[2]) );
  cout << myname << "Done." << endl;
  return 0;
}
