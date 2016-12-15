// channel_map.cxx

// David Adams
// May 2016
//
// Provides interactive access to DUNE channel maps.

#define HAVEROPSUPPORT

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#ifdef HAVEROPSUPPORT
#include "larcoreobj/SimpleTypesAndConstants/readout_types.h"
#endif
#include "larcore/Geometry/Geometry.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "dune/ArtSupport/ArtServiceHelper.h"
#include "dune/DuneInterface/ChannelMappingService.h"

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::istringstream;
using art::ServiceHandle;
using geo::View_t;
using geo::Geometry;
#ifdef HAVEROPSUPPORT
using readout::ROPID;
#endif
using lariov::ChannelStatusService;
using lariov::ChannelStatusProvider;

//**********************************************************************

namespace {

void check(bool ok) {
  if ( ! ok ) {
    cout << "Check failed." << endl;
    abort();
  }
}

string sview(View_t view) {
  if ( view == geo::kU ) return "u";
  if ( view == geo::kV ) return "v";
  if ( view == geo::kW ) return "z";
  if ( view == geo::kZ ) return "z2";
  return "?";
}

}  // end unnamed namespace

//**********************************************************************

int main(int narg, char** argv) {
  string fclname = "standard_reco_dune35tdata";
  bool onToOff = true;
  bool showStatus = true;
  if ( narg > 1 ) {
    string sarg = argv[1];
    if ( sarg == "-h" ) {
      cout << "Usage: " << argv[0] << " [onToOff] [FCLNAME]" << endl;
      cout << "  Conversion is online-to-offline unless onToOff = 0 or false." << endl;
      cout << "  FCLNAME [" << fclname << "] is the FCL file name for the services." << endl;
      cout << "  Current (Dec 2016 FCL names of interest are:" << endl;
      cout << "    standard_reco_dune35tdata" << endl;
      cout << "    standard_reco_dune35tsim" << endl;
      return 0;
    }
    if ( sarg == "0" || sarg == "false" ) onToOff = false;
  }
  if ( narg > 2 ) fclname = argv[2];
  bool isDune = fclname.find("dune") != string::npos;
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
  cout << "Add channel map service." << endl;
  check( ash.addService("ChannelMapService", fclfile, true) == 0 );
  check( ash.addService("ChannelMappingService", fclfile, true) == 0 );
  ash.print();

  cout << line << endl;
  if ( showStatus ) {
    cout << "Add channel status service." << endl;
    check( ash.addService("ChannelStatusService", fclfile, true) == 0 );
    ash.print();
  }

  cout << line << endl;
  cout << "Load services." << endl;
  check( ash.loadServices() == 1 );
  ash.print();
  cout << line << endl;

  // Fetch the services.
  ServiceHandle<ChannelMappingService> hchm;
  const ChannelMappingService* pchm = &*ServiceHandle<ChannelMappingService>();
  const ChannelStatusProvider* pchs = nullptr;
  if ( showStatus ) pchs = ServiceHandle<ChannelStatusService>()->GetProviderPtr();
  const Geometry* pgeo = &*ServiceHandle<Geometry>();

  const vector<string> duneRopNames = { "u", "v", "z1", "z2" };
  while ( true ) {
    cout << "\n" << labels[0] << ": ";
    cout.flush();
    string schan;
    cin >> schan;
    if ( ! isdigit(schan[0]) ) return 0;
    string::size_type ipos = schan.find("-");
    string schan1 = schan.substr(0,ipos);
    string schan2;
    istringstream sschan(schan1);
    int chanin1;
    sschan >> chanin1;
    if ( chanin1 < 0 ) return 0;
    int chanin2 = chanin1;
    if ( ipos != string::npos ) {
      schan2 = schan.substr(ipos+1);
      if ( schan2.size()==0 || ! isdigit(schan2[0]) ) return 0;
      istringstream sschan2(schan2);
      sschan2 >> chanin2;
    }
    for ( int chanin=chanin1; chanin<=chanin2; ++chanin ) {
      int chanout = onToOff ? pchm->offline(chanin) : pchm->online(chanin);
      //int chanon = onToOff ? chanin : chanout;
      int chanoff = onToOff ? chanout : chanin;
      View_t view = pgeo->View(chanoff);
#ifdef HAVEROPSUPPORT
      ROPID ropid = pgeo->ChannelToROP(chanoff);
      int rop = ropid.ROP;
      int apa = ropid.TPCset;
#endif
      cout << labels[1] << "[" << chanin << "] = " << chanout;
#ifdef HAVEROPSUPPORT
      unsigned int icha0 = pgeo->FirstChannelInROP(ropid);
      unsigned int ichaRop = chanoff - icha0;
      // If dune, assume standard plane configuration.
      if ( isDune ) {
        cout << " apa" << apa << duneRopNames[rop] << "-" << ichaRop;
      } else {
        cout << "APA " << apa << ", ROP " << rop << " channel " << ichaRop;
        cout << " view = " << sview(view);
      }
#endif
      if ( showStatus ) {
        cout << " is ";
        if ( !pchs->IsPresent(chanoff) ) {
          cout << "not present";
        } else {
          bool bad = pchs->IsBad(chanoff);
          bool noisy = pchs->IsNoisy(chanoff);
          bool good = !bad && !noisy;
          if ( bad ) {
            cout << "bad";
            if ( noisy ) cout << " and ";
          }
          if ( noisy ) cout << "noisy";
          if ( good ) cout << "good";
        }
      }
      cout << endl;
    }
  }
  return 0;
}

//**********************************************************************
