// DrawResult.h

// David Adams
// January 2016
//
// Struct to hold the result of a draw command.

#ifndef DrawResult_H
#define DrawResult_H

#include <vector>
#include "FFTHist.h"

class TH1;
class TH2;

struct DrawResult {
  double tmin;
  double tmax;
  double sigmin = 0;
  double sigmax = 4096;
  int nsig = -1;
  int status = 0;
  TH2* hdraw = nullptr;
  TH1* hdrawx = nullptr;
  TH1* hdrawy = nullptr;
  std::vector<TH1*> hdrawxChan;
  std::vector<TH1*> hsigChan;
  std::vector<TH1*> hstuckRange;
  std::vector<TH1*> hfreqChan;
  std::vector<TH1*> htpwrChan;
  std::vector<TH1*> hfpwrChan;
  FFTHist* pfft = nullptr;

  // Ctor.
  DrawResult() =default;
  DrawResult(int atmin, int atmax);

  // Signal vs. channel vs. tick.
  TH2* time() const;

  // Signal vs. tick summed over channels.
  TH1* ticks() const;

  // Signal vs. channel summed over ticks.
  TH1* channels() const;

  // Signal vs. tick for one channel.
  TH1* timeChannel(unsigned int chan) const;

  // Histogram of signal for all ticks for one channel.
  // Optionally returns hstuckRange which is the distribution of
  // consecutive stuck bits assuming the signal is direct from ADC.
  // This filled for each stuck bit, not just each stuck-bit range.
  TH1* signalChannel(unsigned int chan, TH1** pphstuckRange =nullptr);

  // Fetch the frequency spectrum for all channels.
  TH2* freq();

  // Fetch the power spectrum for all channels.
  // Normalized so the integral is that for all channels and ticks.
  TH2* power();

  // Fetch the frequency spectrum for a particular channel.
  TH1* freqChannel(unsigned int chan);

  // Fetch the power vs. frequency spectrum for a particular channel.
  TH1* powerChannel(unsigned int chan);

  // Fetch the power vs. frequency spectrum for a particular channel.
  TH1* timePowerChannel(unsigned int chan);


};

#endif
