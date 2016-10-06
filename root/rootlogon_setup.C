// rootlogon_setup.C

// David Adams
// July 2016
//
// Called from rootlogon to set up global symbols for use from
// the Root command line. Note that the required libraries
// must be loaded in rootlogon_load.C.

#include "dxopen.h"

void rootlogon_setup() {
  // Set the detector boundaries.
  drawpars::set35t();
  drawpars::set10ktw();

  // Provide access to the ar service helper.
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  cout << "Art service helper is ash. E.g. ash.print() to list available services." << endl;

  // Open input files listed on the Root command line.
  if ( dxopen(gApplication->InputFiles()) == 0 ) {
    cout << "Opened DXDisplay file " << gFile->GetName() << endl;
  } else {
    cout << "No DXDisplay input file specified." << endl;
  }

}
