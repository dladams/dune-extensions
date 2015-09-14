// test_Geometry.cxx

// David Adams
// September 2015
//
// This test demonstrates how to configure and use the LArSoft Geometry
// service outside the art framework. DUNE geometry and geometry helper
// service are used.
//
// Note the geometry service requires the experiment-specific geometry
// helper with the channel map also be loaded. 
//
// See DXGeo/GeoHelper ctor from geometry name for an alternative to
// loading the geometry service outside the art framework.

#include "Geometry/Geometry.h"

#include <string>
#include <iostream>
#include "DXArt/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

using std::string;
using std::cout;
using std::endl;

int test_Geometry(string gname) {
  const string myname = "test_Geometry: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";
  string scfg;

  cout << line << endl;
  cout << "Fetch art service helper." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();

  cout << line << endl;
  cout << myname << "Add the Geometry service." << endl;
  scfg = "Geometry: { DisableWiresInG4: true GDML: \"dune35t4apa_v5.gdml\" Name: \"" + gname +
         "\" ROOT: \"" + gname + "\" SortingParameters: { DetectorVersion: \"" + gname +
         "\" } SurfaceY: 0 service_type: \"Geometry\" }";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("Geometry", scfg) == 0 );

  cout << line << endl;
  cout << myname << "Add the DUNE geometry helper service (required to load DUNE geometry)." << endl;
  scfg = "ExptGeoHelperInterface: { service_provider: \"DUNEGeometryHelper\" service_type: \"ExptGeoHelperInterface\"}";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("DUNEGeometryHelper", scfg) == 0 );

  cout << line << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << line << endl;
  cout << "Get Geometry service." << endl;
  art::ServiceHandle<geo::Geometry> pgeo;
  cout << myname << "Geometry name: " << pgeo->DetectorName() << endl;

  cout << line << endl;
  return 0;
}

int main() {
  string gname = "dune35t4apa_v5";
  test_Geometry(gname);
  return 0;
}
