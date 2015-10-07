// intProcess.cxx

#include "intProcess.h"

//**********************************************************************

int intProcess(std::string sproc) {
  if ( sproc == "primary"              ) return  0;
  if ( sproc == "Decay"                ) return  1;
  if ( sproc == "muMinusCaptureAtRest" ) return  3;
  if ( sproc == "muPlusCaptureAtRest"  ) return  4;
  if ( sproc == "nCapture"             ) return  5;
  if ( sproc == "CoulombScat"          ) return 10;
  if ( sproc == "ProtonInelastic"      ) return 11;
  if ( sproc == "NeutronInelastic"     ) return 12;
  if ( sproc == "hadElastic"           ) return 13;
  if ( sproc == "PositronNuclear"      ) return 14;
  if ( sproc == "conv"                 ) return 20;
  if ( sproc == "phot"                 ) return 21;
  if ( sproc == "compt"                ) return 22;
  if ( sproc == "PhotonInelastic"      ) return 23;
  if ( sproc == "eIoni"                ) return 30;
  if ( sproc == "eBrem"                ) return 31;
  if ( sproc == "msc"                  ) return 32;
  if ( sproc == "annihil"              ) return 33;
  if ( sproc == "AnnihiToMuPair"       ) return 34;
  if ( sproc == "muIoni"               ) return 40;
  if ( sproc == "muBrems"              ) return 41;
  if ( sproc == "muPairProd"           ) return 42;
  return -1;
}

//**********************************************************************
