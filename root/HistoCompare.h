// HistoCompare.h
//
// David Adams
// November 2015
//
// Class to compare histograms.

#ifndef HistoCompare_H
#define HistoCompare_H

#include <string>
#include "TFile.h"
#include "TH1.h"

class HistoCompare {

public:

  // Ctor from the two input files.
  HistoCompare(std::string afname1, std::string afname2);

  // Compare histogram with same name in the two files.
  // Returns the number of differing bins (nbad).
  int compare(std::string hname);

  // Make comparison for all 35t planes: apa0u, ..., apa3z2
  // Returns the number of channels that differ.
  // If evt1 > 0, comparison is made for hevt1_hpre, ..., hevt2_hpre
  // Then returns the number of histograms that differ.
  int compare35t(std::string hpre, int evt1=0, int evt2=1);

  // Data.
  std::string fname1;
  std::string fname2;
  int dbg;
  int nbin;
  int nbinbad;
  int nhst; 
  int nhstbad; 

};

#endif

