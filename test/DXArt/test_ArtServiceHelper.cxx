// test_ArtServiceHelper.cxx

#include "DXArt/ArtServiceHelper.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>
#include "art/Framework/Services/Registry/ServiceRegistry.h"
#include "art/Framework/Services/Optional/RandomNumberGenerator.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/System/TriggerNamesService.h"

using std::string;
using std::cout;
using std::endl;
using std::hex;
using art::ServiceRegistry;

int test_ArtServiceHelper() {
  const string myname = "test_ArtServiceHelper: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";
  string scfg;

  cout << line << endl;
  cout << myname << "Retrieve service helper.";
  ArtServiceHelper& ash = ArtServiceHelper::instance();

  ash.print();

  cout << line << endl;
  cout << myname << "Add TFileService" << endl;
  scfg = "TFileService: { fileName: \"single35t_hist.root\" service_type: \"TFileService\"}";
  assert( ash.addService("TFileService", scfg) == 0 );
  ash.print();

  cout << line << endl;
  cout << myname << "Try to add TFileService again" << endl;
  assert( ash.addService("TFileService", scfg) != 0 );
  ash.print();

  cout << line << endl;
  cout << myname << "Add RandomNumberGenerator" << endl;
  scfg = "RandomNumberGenerator: { service_type: \"RandomNumberGenerator\" }";
  ash.addService("RandomNumberGenerator", scfg);
  ash.print();
  assert( ash.serviceNames().size() == 2 );
  assert( ash.serviceStatus() == 0 );
  
  cout << line << endl;
  cout << myname << "Full configuration:" << endl;
  cout << myname << ash.fullServiceConfiguration() << endl;

  cout << line << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << line << endl;
  cout << "Fetch service registry." << endl;
  ServiceRegistry& sr = ServiceRegistry::instance();
  cout << myname << "Service registry address:  " << hex << &sr << endl;
  //cout << "Check TriggerNamesService is available." << endl;
  //assert(sr.isAvailable<TriggerNamesService>());
  cout << line << endl;
  cout << "Check RandomNumberGenerator is available." << endl;
  assert(sr.isAvailable<art::RandomNumberGenerator>());
  cout << line << endl;
  cout << "Check TFileService is available." << endl;
  assert(sr.isAvailable<art::TFileService>());

  cout << line << endl;
  cout << "Check RandomNumberGenerator is accessible." << endl;
  sr.get<art::RandomNumberGenerator>();
  cout << line << endl;
  cout << "Check TFileService is accessible" << endl;
  sr.get<art::TFileService>();   // This raise an exception!
  cout << line << endl;
  cout << "Check TriggerNameService is accessible" << endl;
  sr.get<art::TriggerNamesService>();

  cout << line << endl;
  cout << myname << "Done." << endl;
  return 0;
}

int main() {
  return test_ArtServiceHelper();
}
