// test_ChannelTickHistCreator.cxx

// David Adams
// Spptember 2015
//
// Test script for Range.

#include "DXUtil/ChannelTickHistCreator.h"

#include <string>
#include <iostream>
#include <cassert>
#include "AXService/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"

using std::string;
using std::cout;
using std::endl;
typedef ChannelTickHistCreator::TickRange TickRange;

int test_ChannelTickHistCreator() {
  const string myname = "test_ChannelTickHistCreator: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";

  // Provide access to the art TFile service.
  cout << line << endl;
  cout << myname << "Retrieve service helper.";
  ArtServiceHelper& ash = ArtServiceHelper::instance();
  cout << myname << "Add TFileService" << endl;
  string scfg = "fileName: \"cthc_test.root\"";
  assert( ash.addService("TFileService", scfg) == 0 );
  cout << myname << "Load services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  // Fetch the TFile service.
  art::ServiceHandle<art::TFileService> hfs;
  art::TFileService* pfs = hfs.operator->();
  assert( pfs != nullptr );
  TFile& file = pfs->file();
  assert( file.IsOpen() );
  file.ls();

  // Create histogram creator.
  cout << myname << line << endl;
  cout << myname << "Create histogram creator." << endl;
  ChannelTickHistCreator cthc(*pfs, "123", 0.0, 1000, "ADC counts", 0, 200, 8, 10);

  cout << myname << line << endl;
  cout << myname << "Create a histogram." << endl;
  cthc.create("apa1x1", 0, 128, "APA 1x1", "trk01", "Track 1", TickRange(80, 290));
  file.ls();
  TObject* pobj = file.Get("h123trk01_apa1x1");
  assert( pobj != nullptr );
  assert( pobj != nullptr );
  TH2* ph2 = dynamic_cast<TH2*>(pobj);
  int nbinx = ph2->GetXaxis()->GetNbins();
  int nbiny = ph2->GetYaxis()->GetNbins();
  assert( ph2 != nullptr );
  cout << myname << "Histogram ranges: "  << endl;
  cout << myname << "  Xmin: " << ph2->GetXaxis()->GetXmin() << endl;
  cout << myname << "  Xmax: " << ph2->GetXaxis()->GetXmax() << endl;
  cout << myname << "  Ymin: " << ph2->GetYaxis()->GetXmin() << endl;
  cout << myname << "  Ymax: " << ph2->GetYaxis()->GetXmax() << endl;
  cout << myname << "  Zmin: " << ph2->GetMinimum() << endl;
  cout << myname << "  Zmax: " << ph2->GetMaximum() << endl;
  cout << myname << "  # X-bin: " << nbinx << endl;
  cout << myname << "  # Y-bin: " << nbiny << endl;
  cout << myname << "  # contours: " << ph2->GetContour() << endl;
  assert( ph2->GetXaxis()->GetXmin() ==  50 );
  assert( ph2->GetXaxis()->GetXmax() == 300 );
  assert( ph2->GetYaxis()->GetXmin() ==   0 );
  assert( ph2->GetYaxis()->GetXmax() == 128 );
  assert( ph2->GetMinimum() ==   0 );
  assert( ph2->GetMaximum() == 200 );
  assert( ph2->GetContour() == 8 );
  assert( nbinx ==  25 );
  assert( nbiny == 128 );

  cout << myname << line << endl;
  cout << myname << "Close services." << endl;
  ash.close();
  cout << "Done." << endl;
  return 0;
}

int main() {
  return test_ChannelTickHistCreator();
}
