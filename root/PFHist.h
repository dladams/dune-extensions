// PFHist.h

#ifndef PFHist_H
#define PFHist_H

class TH2;

class PFHist {

public:

  PFHist(TH2* ahin);

  TH2* hin;
  TH2* hout;

};

#endif
