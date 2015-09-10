// test_TFileService.cxx

#include "art/Framework/Services/System/TriggerNamesService.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include <string>
#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceRegistry.h"
#include "art/Framework/EventProcessor/ServiceDirector.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::unique_ptr;
using fhicl::ParameterSet;
using fhicl::make_ParameterSet;
using art::ActivityRegistry;
using art::ServiceRegistry;
using art::ServiceDirector;
using art::ServiceToken;
using art::TriggerNamesService;
using art::TFileService;
using art::TFileDirectory;

int test_TFileService() {
  const string myname = "test_TFileService: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";

  cout << line << endl;
  cout << "Create activity registry." << endl;
  ActivityRegistry ar;

  // Create vector of service configurations.
  string scfgServices;

  cout << line << endl;
  cout << "Configure TFile service." << endl;
  {
    string scfg = "fileName: \"test.root\" service_type: \"TFileService\"";
    string sscfg = "TFileService: { " + scfg + " }";
    cout << sscfg << endl;
    scfgServices += sscfg;
  }

  // Use ServiceDirector to create and register user services.
  // See art/Framework/EventProcessor/EventProcessor.cc
  cout << line << endl;
  cout << "Create service director." << endl;
  ParameterSet cfgServices;
  make_ParameterSet(scfgServices, cfgServices);
  ServiceToken serviceToken;
  ServiceDirector director(cfgServices, ar, serviceToken);

  // Add the TriggerNames service.
  // TFileService needs this to find the process name.
  cout << "Configure and register the trigger names service." << endl;
  ParameterSet cfgTriggerNamesService;
  string scfgTriggerNamesService = "process_name: \"myproc\"";
  cout << scfgTriggerNamesService << endl;
  make_ParameterSet(scfgTriggerNamesService, cfgTriggerNamesService);
  vector<string> tns;
  unique_ptr<TriggerNamesService> ptns(new TriggerNamesService(cfgTriggerNamesService, tns));
  director.addSystemService(std::move(ptns));

  // Make the services available
  ServiceRegistry::Operate operate(serviceToken);

  cout << line << endl;
  ServiceRegistry& sr = ServiceRegistry::instance();
  cout << "Check TriggerNamesService is available." << endl;
  assert(sr.isAvailable<TriggerNamesService>());
  cout << "Check TFileService is available." << endl;
  assert(sr.isAvailable<TFileService>());

  cout << "Check TriggerNameService is accessible" << endl;
  sr.get<TriggerNamesService>();
  cout << "Check TFileService is accessible" << endl;
  sr.get<TFileService>();   // This raise an exception!

  cout << line << endl;
  cout << "Get TFile service." << endl;
  art::ServiceHandle<art::TFileService> pfs;
  //TFileService& fs = sr.get<TFileService>();
  cout << "Check if TFile is open." << endl;
  assert(pfs->file().IsOpen());
  cout << "Retrieve TFile name." << endl;
  cout << "File name: " << pfs->file().GetName() << endl;

  cout << line << endl;
  cout << "Add a histogram." << endl;
  TH1* ph1 = pfs->make<TH1F>("hist1", "Histogram 1", 100, 0, 100);

  cout << line << endl;
  cout << "Add a histogram in a subdirectory." << endl;
  TFileDirectory fd2 = pfs->mkdir("mydir", "My directory");
  TH1* ph2 = fd2.make<TH1F>("hist2", "Histogram 2", 100, 0, 100);

  cout << line << endl;
  cout << "Fill histograms." << endl;
  int nent = 10000;
  for ( int ent=0; ent<nent; ++ent ) {
    double val = 10.0/double(nent)*ent;
    val *= val;
    if ( val < 10.0 ) ph1->Fill(val);
    else ph2->Fill(val);
  }

  cout << line << endl;
  cout << "Done." << endl;
  cout << "Output file: " << pfs->file().GetName() << endl;
  return 0;
}

int main() {
  string line = "-----------------------------";
  system("rm -f test.root");
  system("ls -ls");
  TFile* pf1 = TFile::Open("test.root", "READ");
  assert( pf1 == nullptr );
  int rstat = test_TFileService();
  assert( rstat == 0 );
  system("ls -ls");
  cout << line << endl;
  cout << "Checking output root file" << endl;
  TFile* pf2 = TFile::Open("test.root", "READ");
  assert( pf2 != nullptr && pf2->IsOpen() );
  pf2->ls();
  TObject* po1 = pf2->Get("hist1");
  TObject* po2 = pf2->Get("hist2");
  TH1F* ph1 = dynamic_cast<TH1F*>(po1);
  assert( po1 != nullptr );
  assert( ph1 != nullptr );
  assert( po2 == nullptr );
  cout << "Changing to bad directory" << endl;
  assert( ! pf2->cd("nodir") );
  cout << line << endl;
  cout << "Changing file directory" << endl;
  assert( pf2->cd("mydir") );
  pf2->ls();
  pf2->pwd();
  po1 = pf2->Get("hist1");
  po2 = pf2->Get("hist2");
  TH1F* ph2 = dynamic_cast<TH1F*>(po2);
  assert( po2 != nullptr );
  assert( ph2 != nullptr );
  assert( po1 == nullptr );
  return rstat;
}

