// FFTHist.h

#ifndef FFTHist_H
#define FFTHist_H

#include <vector>

class TH1;
class TH2;
class TFFTRealComplex;

class FFTHist {

public:

  typedef vector<TFFTRealComplex*> FFTVector;

  FFTHist(TH2* hin, int tmin =0, int tmax =0, double ftick =0.0);

  TH2* maketime(int tmin, int tmax);

  bool phase0;        // If true phases are relative to tick 0. Otherwise tmin.
  double tmin, tmax;  // Tick range used for FFT
  TH2* htime0;        // Input signal histogram (channel vs. tick).
  TH2* hfreq;
  TH2* hpower;
  TH2* hphase;
  TH1* hptime;
  TH1* hpfreq;
  TH2* htime;
  FFTVector ffts;    // FFT for each channel

};

#endif
