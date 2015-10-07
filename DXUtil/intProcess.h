// intProcess.h

#ifndef intProcess_H
#define intProcess_H

// David Adams
// March 2015
//
// Converts a NUTOOLS McParticle process to an integer.
//
//  0 - primary
//  1 - Decay
//  3 - muMinusCaptureAtRest
//  4 - muPlusCaptureAtRest
//  5 - nCapture
// 10 - CoulombScat
// 11 - ProtonInelastic
// 12 - NeutronInelastic
// 13 - hadElastic
// 14 - PositronNuclear
// 20   conv                  G4GammaConversion
// 21   phot                  G4PhotoElectricEffect
// 22   compt                 G4ComptonScattering
// 23   PhotonInelastic       G4PhotoNuclearProcess  (aka photonNuclear?)
// 30   eIoni                 G4eIonisation
// 31   eBrem                 G4eBremsstrahlung
// 32   msc                   G4eMultipleScattering
// 33   annihil               G4eplusAnnihilation
// 34   AnnihiToMuPair        G4AnnihiToMuPair
// 40   muIoni                G4MuIonisation
// 41   muBrems               G4MuBremsstrahlung
// 42   muPairProd            G4MuPairProduction
// -1 - Anything else

#include <string>

int intProcess(std::string sproc);

#endif
