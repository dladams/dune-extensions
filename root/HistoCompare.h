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
#include "TFile.h"

class HistoCompare {

public:

  // Ctor from the two input files.
  // dbg = 1 shows values for differing channels
  // dbg = 2 shows values for all channels
  // If pfile is not null, results are added to that file.
  // If it is null, a file is opened in ctor and closed in dtor.
  HistoCompare(std::string afname1, std::string afname2, int adbg =0, TFile* pfile =0);

  // Dtor.
  ~HistoCompare();

  // Compare histogram with same name in the two files.
  // Returns the number of differing bins (nbad).
  // Bins are different if fdiff > fdiffmax.
  // fdiff = abs(x1-x2)/(0.5*abs(x1+x2))
  // If pmaxfdiff, the max fdiff is stored there.
  int compare(std::string hname, double fdiffmax =0.0, double* pmaxfdiff =nullptr);

  // Make comparison for all planes: apa0u, ..., apa3z2
  // For detector sdet = "35t" or "fd126".
  // Returns the number of channels that differ.
  // If evt1 > 0, comparison is made for hevt1_hpre, ..., hevt2_hpre
  // Then returns the number of histograms that differ.
  int compareall(std::string sdet, std::string hpre, int evt1=0, int evt2=1, double fdiffmax =0.0);

  // Print the configuration.
  void print() const;

  // Data.
  std::string fname1;
  std::string fname2;
  int dbg;
  int nbin;
  int nbinbad;
  int nhst; 
  int nhstbad; 

  // Histogram with differences.
  TFile* m_pfile;
  bool m_closefile;
  TH1* m_phdiff;

};

#endif

