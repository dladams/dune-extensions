// test_DetectorProperties.cxx

// David Adams
// September 2015
//
// This test demonstrates how to configure and use the LArSoft DetectorProperties
// service outside the art framework.

#include "Utilities/DetectorProperties.h"

#include <string>
#include <iostream>
#include "DXArt/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Core/EngineCreator.h"
#include "CLHEP/Random/RandomEngine.h"

using std::string;
using std::cout;
using std::endl;

int test_DetectorProperties(string gname) {
  const string myname = "test_DetectorProperties: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  string line = "-----------------------------";
  string scfg;

  cout << myname << line << endl;
  cout << myname << "Fetch art service helper." << endl;
  ArtServiceHelper& ash = ArtServiceHelper::instance();

  cout << myname << line << endl;
  cout << myname << "Add the DetectorProperties service." << endl;
  scfg = "DetectorProperties: { ElectronsToADC: 6.8906513e-3 NumberTimeSamples: 3200 ReadOutWindowSize: 3200 TimeOffsetU: 0 TimeOffsetV: 0 TimeOffsetZ: 0 service_type: \"DetectorProperties\"}";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("DetectorProperties", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the TimeService service." << endl;
  scfg = "TimeService: { ClockSpeedExternal: 3.125e1 ClockSpeedOptical: 128 ClockSpeedTPC: 2 ClockSpeedTrigger: 16 DefaultBeamTime: 0 DefaultTrigTime: 0 FramePeriod: 1600 G4RefTime: 0 InheritClockConfig: false TrigModuleName: \"\" TriggerOffsetTPC: 0 service_type: \"TimeService\" }";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("TimeService", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Add the DatabaseUtil service." << endl;
  scfg = "DatabaseUtil: { DBHostName: \"fnalpgsdev.fnal.gov\" DBName: \"dune_dev\" DBUser: \"dune_reader\" PassFileName: \".lpswd\" Port: 5438 ShouldConnect: false TableName: \"main_run\" ToughErrorTreatment: false service_type: \"DatabaseUtil\"}";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("DatabaseUtil", scfg) == 0 );

  cout << myname << line << endl;
  cout << myname << "Load the services." << endl;
  assert( ash.loadServices() == 1 );
  ash.print();

  cout << myname << line << endl;
  cout << myname << "Get DetectorProperties service." << endl;
  art::ServiceHandle<util::DetectorProperties> pdetsrv;

  cout << myname << line << endl;
  cout << myname << "Use DetectorProperties service." << endl;
  cout << myname << "    SamplingRate: " << pdetsrv->SamplingRate() << endl;
  cout << myname << "  ElectronsToADC: " << pdetsrv->ElectronsToADC() << endl;

  // Close services.
  cout << myname << line << endl;
  cout << myname << "Close services." << endl;
  ArtServiceHelper::close();

  cout << myname << line << endl;
  return 0;
}

int main() {
  string gname = "dune35t4apa_v5";
  test_DetectorProperties(gname);
  return 0;
}
