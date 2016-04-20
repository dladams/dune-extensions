// chanmap.cxx

// David Adams
// April 2016
//
// Display channel map.

#include "dune/ArtSupport/ArtServiceHelper.h"
#include "larcore/Geometry/Geometry.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using std::istringstream;
using std::vector;
using std::setw;
using art::ServiceHandle;
using geo::Geometry;

int chanmap(unsigned int chan0, unsigned int nchan) {
  const string myname = "chanmap: ";
  string line = "-----------------------------";

  cout << myname << line << endl;
  cout << myname << "Create top-level FCL." << endl;
  string fclfile = "chanmap.fcl";
  ofstream fout(fclfile.c_str());
  fout << "#include \"geometry_dune.fcl\"" << endl;
  fout << "services.Geometry: @local::dune35t_geo" << endl;
  fout << "services.ExptGeoHelperInterface: @local::dune_geometry_helper" << endl;
  fout.close();

  cout << myname << "Fetch art service helper." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Add geometry service." << endl;
  assert( ash.addService("Geometry", fclfile, true) == 0 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Add the DUNE geometry helper service (required to load DUNE geometry)." << endl;
  assert( ash.addService("ExptGeoHelperInterface", fclfile, true) == 0 );

  cout << myname << line << endl;
  cout << myname << "Load services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Fetch geometry service." << endl;
  ServiceHandle<Geometry> hgeo;
  cout << myname << "Detector: " << hgeo->DetectorName() << endl;

  unsigned int hchan  = 10;
  unsigned int hview  = 10;
  for ( unsigned int chan=chan0; chan<chan0+nchan; ++chan ) {
    cout << setw(hchan) << chan
         << setw(hview) << hgeo->View(chan)
         << endl;
  }

  cout << myname << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}

int main(int argc, char* argv[]) {
  unsigned int chan0 = 0;
  unsigned int nchan = 32;
  if ( argc > 1 ) {
    istringstream sin(argv[1]);
    sin >> chan0;
  }
  if ( argc > 2 ) {
    istringstream sin(argv[2]);
    sin >> nchan;
  }
  return chanmap(chan0, nchan);
}
