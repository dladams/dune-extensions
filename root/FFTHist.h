// FFTHist.h

#ifndef FFTHist_H
#define FFTHist_H

class TH1;
class TH2;

class FFTHist {

public:

  FFTHist(TH2* hin, int tmin =0, int tmax =0, double ftick =0.0);

  TH2* maketime(int tmin, int tmax);

  TH2* htime0;
  TH2* hfreq;
  TH2* hphase;
  TH1* hptime;
  TH1* hpfreq;
  TH2* htime;

};

#endif
