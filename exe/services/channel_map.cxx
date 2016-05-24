// channel_map.cxx

// David Adams
// May 2016
//
// Provides interqctive access to DUNE channel maps.

#include <string>
#include <iostream>
#include <sstream>
#include "dune/ArtSupport/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "dune/DuneInterface/ChannelMappingService.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::istringstream;
using art::ServiceHandle;

//**********************************************************************

namespace {

void check(bool ok) {
  if ( ! ok ) {
    cout << "Check failed." << endl;
    abort();
  }
}

}  // end unnamed namespace

//**********************************************************************

int main(int narg, char** argv) {
  string fclname = "standard_reco_dune35t";
  bool onToOff = true;
  if ( narg > 1 ) {
    string sarg = argv[1];
    if ( sarg == "-h" ) {
      cout << "Usage: " << argv[0] << " [onToOff] [FCLNAME]" << endl;
      cout << "  Conversion is online-to-offline unless onToOff = 0 or false." << endl;
      cout << "  FCLNAME [standard_reco_dune35t] is the FCL file name for the services." << endl;
      cout << "  Current (May 2016 FCL names of interest are:" << endl;
      cout << "    standard_reco_dune35t" << endl;
      return 0;
    }
    if ( sarg == "0" || sarg == "false" ) onToOff = false;
  }
  if ( narg > 2 ) fclname = argv[2];
  string fclfile = fclname + ".fcl";
  cout << "FCL file: " << fclfile << endl;
  if ( onToOff ) cout << "Converting online to offline." << endl;
  else           cout << "Converting offline to online." << endl;
  string labels[2] = {"  Online", " Offline"};
  if ( ! onToOff ) {
    string stmp = labels[0];
    labels[0] = labels[1];
    labels[1] = stmp;
  }
  string line = "==========================================";
  cout << line << endl;

  cout << "Fetch art service helper." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  ash.setLogLevel(2);
  ash.print();

  cout << line << endl;
  cout << "Add geometry service." << endl;
  check( ash.addService("Geometry", fclfile, true) == 0 );
  ash.print();

  cout << line << endl;
  cout << "Add the DUNE geometry helper service (required to load DUNE geometry)." << endl;
  check( ash.addService("ExptGeoHelperInterface", fclfile, true) == 0 );

  cout << line << endl;
  cout << "Add channel map." << endl;
  check( ash.addService("ChannelMapService", fclfile, true) == 0 );
  check( ash.addService("ChannelMappingService", fclfile, true) == 0 );
  ash.print();

  cout << line << endl;
  cout << "Load services." << endl;
  check( ash.loadServices() == 1 );
  ash.print();
  cout << line << endl;

  // Fetch the services.
  ServiceHandle<ChannelMappingService> hchm;
  const ChannelMappingService* pchm = &*hchm;

  while ( true ) {
    cout << "\n" << labels[0] << ": ";
    cout.flush();
    string schan;
    cin >> schan;
    if ( ! isdigit(schan[0]) ) return 0;
    istringstream sschan(schan);
    int chanin;
    sschan >> chanin;
    if ( chanin < 0 ) return 0;
    int chanout = onToOff ? pchm->offline(chanin) : pchm->online(chanin);
    cout << labels[1] << "[" << chanin << "] = " << chanout << endl;
  }
  return 0;
}

//**********************************************************************
