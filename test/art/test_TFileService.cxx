// test_TFileService.cxx

#include "art/Framework/Services/Optional/TFileService.h"

#include <string>
#include <iostream>
#include "TFile.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceRegistry.h"

using std::string;
using std::cout;
using std::endl;
using fhicl::ParameterSet;
using fhicl::make_ParameterSet;
using art::ActivityRegistry;
using art::ServiceRegistry;
using art::ServiceToken;
using art::TFileService;

int main() {
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

  cout << line << endl;
  cout << "Configure TFile service." << endl;
  ServiceRegistry::ParameterSets cfgs;
  {
    string scfg = "fileName: \"test.root\" service_type: \"TFileService\"";
    cout << scfg << endl;
    ParameterSet cfg;
    make_ParameterSet(scfg, cfg);
    cfgs.push_back(cfg);
  }

  cout << line << endl;
  cout << "Create and configure services manager." << endl;
  ServiceRegistry& sr = ServiceRegistry::instance();
  ServiceToken stok = ServiceRegistry::createSet(cfgs, ar);
  ServiceRegistry::Operate sop(stok);

  cout << line << endl;
  cout << "Check file service is available." << endl;
  cout << typeid(TFileService).name() << endl;
  assert(sr.isAvailable<TFileService>());

  cout << "Check file service get." << endl;
  cout << typeid(TFileService).name() << endl;
  sr.get<TFileService>();   // This raise an exception!

  cout << line << endl;
  cout << "Get TFile service." << endl;
  TFileService& fs = sr.get<TFileService>();
  cout << "Check if TFile is open." << endl;
  assert(fs.file().IsOpen());
  cout << "Retrieve TFile name." << endl;
  cout << "File name: " << fs.file().GetName() << endl;

  cout << line << endl;
  cout << "Done." << endl;
  return 0;
}
  
