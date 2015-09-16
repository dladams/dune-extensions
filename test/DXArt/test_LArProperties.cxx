// test_LArProperties.cxx

// David Adams
// September 2015
//
// This test demonstrates how to configure and use the LArSoft LArProperties
// service outside the art framework.

#include "Utilities/LArProperties.h"

#include <string>
#include <iostream>
#include "DXArt/ArtServiceHelper.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Core/EngineCreator.h"
#include "CLHEP/Random/RandomEngine.h"

using std::string;
using std::cout;
using std::endl;

int test_LArProperties(string gname) {
  const string myname = "test_LArProperties: ";
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
  cout << myname << "Add the LArProperties service." << endl;
  scfg = "LArProperties: {\n";
  scfg += "AbsLengthEnergies: [ 4, 5, 6, 7, 8, 9,10, 11]\n";
  scfg += "AbsLengthSpectrum: [ 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000 ]\n";
  scfg += "AlphaScintYield: 16800 AlphaScintYieldRatio: 5.6e-1\n";
  scfg += "Argon39DecayRate: 0\n";
  scfg += "AtomicMass: 3.9948e1\n";
  scfg += "AtomicNumber: 18\n";
  scfg += "Efield: [5e-1, 6.66e-1, 8e-1]\n";
  scfg += "ElectronScintYield: 20000 ElectronScintYieldRatio: 0.27 Electronlifetime: 3000\n";
  scfg += "EnableCerenkovLight: false\n";
  scfg += "ExcitationEnergy: 188\n";
  scfg += "FastScintEnergies: [ 6, 6.7, 7.1, 7.4, 7.7, 7.9, 8.1, 8.4, 8.5, 8.6, 8.8, 9, 9.1, 9.4, 9.8, 10.4, 10.7]\n";
  scfg += "FastScintSpectrum: [0, 0.04, 0.12, 0.27, 0.44, 0.62, 0.80, 0.91, 0.92, 0.85, 0.70, 0.50, 0.31, 0.13, 0.04, 0.01, 0]\n";
  scfg += "KaonScintYield: 24000 KaonScintYieldRatio: 0.23\n";
  scfg += "MuonScintYield: 24000 MuonScintYieldRatio: 0.23\n";
  scfg += "PionScintYield: 24000 PionScintYieldRatio: 0.23\n";
  scfg += "ProtonScintYield: 19200 ProtonScintYieldRatio: 0.29\n";
  scfg += "RIndexEnergies: [1.9, 2.934, 3.592, 5.566, 6.694, 7.54, 8.574, 9.044, 9.232, 9.42, 9.514, 9.608,";
  scfg += " 9.702, 9.796, 9.89, 9.984, 10.08, 10.27, 10.45, 10.74, 10.92]\n";
  scfg += "RIndexSpectrum: [1.232, 1.236, 1.24, 1.261, 1.282, 1.306, 1.353, 1.387, 1.404, 1.423, 1.434, 1.446,";
  scfg += " 1.459, 1.473, 1.488, 1.505, 1.524, 1.569, 1.627, 1.751, 1.879]\n";
  scfg += "RadiationLength: 19.55\n";
  scfg += "RayleighEnergies: [2.95, 3.1, 3.54, 4.13, 4.96, 5.76, 6.53, 8.0, 8.4, 8.8, 9.0, 9.2, 9.4, 9.5, 9.6,";
  scfg += "9.7, 9.8, 9.9, 10.0, 10.2, 10.4, 10.6, 10.8]\n";
  scfg += "RayleighSpectrum: [43400, 34900, 20000, 10800, 4800, 2250, 1170, 367, 257, 174, 141, 114, 90.6, 80.5,";
  scfg += " 71.1, 62.6, 54.7, 47.4, 40.7, 29.0, 19.4, 12.0, 6.66]\n";
  scfg += "ReflectiveSurfaceDiffuseFractions: [ [0.5, 0.5, 0.5] ]\n";
  scfg += "ReflectiveSurfaceEnergies: [7, 9, 10]\n";
  scfg += "ReflectiveSurfaceNames: [ \"STEEL_STAINLESS_Fe7Cr2Ni\"]\n";
  scfg += "ReflectiveSurfaceReflectances: [ [0.25, 0.25, 0.25] ]\n";
  scfg += "ScintBirksConstant: 0.069\n";
  scfg += "ScintByParticleType: false\n";
  scfg += "ScintFastTimeConst: 6\n";
  scfg += "ScintPreScale: 3e-2\n";
  scfg += "ScintResolutionScale: 1\n";
  scfg += "ScintSlowTimeConst: 1590\n";
  scfg += "ScintYield: 24000\n";
  scfg += "ScintYieldRatio: 0.30\n";
  scfg += "SlowScintEnergies: [ 6.0,  6.7,  7.1,  7.4,  7.7, 7.9,  8.1,  8.4,  8.5,  8.6,  8.8,  9.0,  9.1,  9.4,  9.8,  10.4,  10.7]\n";
  scfg += "SlowScintSpectrum: [ 0.0,  0.04, 0.12, 0.27, 0.44, 0.62, 0.80, 0.91, 0.92, 0.85, 0.70, 0.50, 0.31, 0.13, 0.04,  0.01, 0.0]\n";
  scfg += "SternheimerA: 0.1956 SternheimerCbar: 5.2146 SternheimerK: 3.0 SternheimerX0: 0.20 SternheimerX1: 3.0\n";
  scfg += "Temperature: 87\n";
  scfg += "service_type: \"LArProperties\"}";
  cout << myname << "Configuration: " << scfg << endl;
  assert( ash.addService("LArProperties", scfg) == 0 );

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
  cout << myname << "Get LArProperties service." << endl;
  art::ServiceHandle<util::LArProperties> plarsrv;

  cout << myname << line << endl;
  cout << myname << "Use LArProperties service." << endl;
  cout << myname << "    LAr Density: " << plarsrv->Density() << endl;
  cout << myname << "  DriftVelocity: " << plarsrv->DriftVelocity() << endl;

  // Close services.
  cout << myname << line << endl;
  cout << myname << "Close services." << endl;
  ArtServiceHelper::close();

  cout << myname << line << endl;
  return 0;
}

int main() {
  string gname = "dune35t4apa_v5";
  test_LArProperties(gname);
  return 0;
}
