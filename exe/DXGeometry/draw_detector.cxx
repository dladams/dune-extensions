// draw_detector.cxx

// David Adams
// August 2015
//
// Draw DUNE geometries.

#include "TApplication.h"
#include "TRint.h"
#include "DXGeometry/LArGeoManager.h"

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

int main(int narg, char** argv) {
  string gname = "dune10kt_v1";
  bool useDefault = false;
  if ( narg > 1 ) gname = argv[1];
  if ( narg > 2 ) useDefault = true;
  if ( gname == "-h" || gname == "help" ) {
    cout << "Usage: " << argv[0] << " [NAME] [DOPT]" << endl;
    cout << "  GNAME [dune10kt_v1] is the geometry name (i.e. from GNAME.gdml)" << endl;
    cout << "  Current (Oct 2015) geometries of interest are:" << endl;
    cout << "    dune10kt_v1" << endl;
    cout << "    dune10kt_v1_workspace" << endl;
    cout << "    dune35t4apa_v5" << endl;
    cout << "  DOPT [1] is the drawing option:" << endl;
    cout << "    0 - volumes from the GDML file (opaque)" << endl;
    cout << "    1 - Semi-transparent TPC volumes only" << endl;
    return 0;
  }
  string line = "==========================================";
  cout << line << endl;
  cout << "Drawing detector " << gname << endl;
  cout << "For help: " << argv[0] << " help" << endl;
  cout << "To exit, use Eve menus: \"Browser\", \"Quit Root\"" << endl;
  cout << line << endl;
  //TRint* ptapp = new TRint("myapp", &narg, argv, nullptr, -1);
  TApplication* ptapp = new TApplication("myapp", &narg, argv, nullptr, -1);
  GeoHelper gh(gname);
  if ( useDefault ) {
    LArGeoManager lgm;
    lgm.draw();
  } else {
    LArGeoManager lgm(gh, 1);
    lgm.draw();
  }
  cout << "To quit, use Browser->\"Quit Root\" or Command: .q" << endl;
  ptapp->Run(true);
  ptapp->Terminate(0);
  cout << endl;   // Root 5.34 leaves an unflushed line.
  return 0;
}
