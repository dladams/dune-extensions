// FFTHist.h

// David Adams
// October 2016
//
// Constructs FT histograms from input signal in a 2D histogram vs. channel vs tick.
// Does an independent FFT for each channel.
//
// Output histograms exclude conjugate frequencies. I.e. for nt input ticks, there are
//   nk = nt/2 + 1 
// frequency bins, i.e. congugate frequencies are not included.
//
// For the missing conjugate ticks, i.e. k > nk:
//   the magnitude is y_k = y_(nt-k)
//   and phase is phi_k = phi_(nt-k) + pi
// Power is the square of the signal and includes the conjugate contribution in frequency
// histograms so that the total power for each channel should be the same in the time
// and frequency plots.
//
// Following Root convention, if ix and iy are the Root bin indices:
//   iy = channel + 1, e.g. channel 100 is in bin 101
//   ix = it+1 or ik+1, e.g. tick 200 is in bin 201
// Bin 0 holds underflows and should be empty.

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
  TH2* hfreq;         // FT magnitude vs. (channel vs. frequency)
  TH2* hpower;        // FT power vs. (channel vs. frequency)
  TH2* hphase;        // FT phase vs. (channel vs. frequency)
  TH1* hptime;        // Input power summed over channels vs. tick
  TH1* hpfreq;        // FT power summed over channels vs. frequency
  TH2* htime;         // Input signal histogram reconstructed from the FT.
  FFTVector ffts;     // FFT for each channel

};

#endif
