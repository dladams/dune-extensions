// DrawResult.h

// David Adams
// January 2016
//
// Struct to hold the result of a draw command.

#ifndef DrawResult_H
#define DrawResult_H

#include <vector>
#include <map>
#include "FFTHist.h"

class TH1;
class TH2;

struct DrawResult {
  std::string filename;
  std::string name;
  double tmin;
  double tmax;
  double sigmin = 0;
  double sigmax = 4096;
  double truncsigma = 3.0;
  int nsig = -1;
  int status = 0;
  int stuckthresh = 1;
  int samethresh = 3;
  // If true pedestal is added back when evaluating ADC modulus and struck bits.
  bool havePedestal = false;
  TH2* hdraw = nullptr;
  TH1* hdrawx = nullptr;
  TH1* hdrawy = nullptr;
  TH1* hchanstat = nullptr;
  std::vector<TH1*> hdrawxChan;
  std::vector<TH1*> hsigChan;    // Signal histogram.
  std::vector<TH1*> hsinChan;    // Not-stick signal histogram.
  std::vector<TH1*> hstuckRange;
  std::vector<TH1*> hsameRange;
  std::vector<TH1*> hmodChan;
  std::vector<TH1*> hfreqChan;
  std::vector<TH1*> htpwrChan;
  std::vector<TH1*> hfpwrChan;
  FFTHist* pfft = nullptr;
  TH1* hmean = nullptr;
  TH1* hrms = nullptr;
  TH1* hrmt = nullptr;
  TH1* hmen = nullptr;      // Mean w/o sticky codes
  TH1* hrmn = nullptr;      // RMS w/o sticky codes
  TH1* hrnt = nullptr;      // Truncated RMS w/o sticky codes.
  std::vector<TH1*> hstuckThresh;
  std::vector<TH1*> hsameThresh;
  std::vector<double> pedestals;
  std::vector<short> chanstats;
  std::map<std::string, TH2*> hrmsWindow;   // Histograms of RMS vs. channel vs. binned tick
  std::map<std::string, TH2*> hmeanWindow;  // Histograms of mean vs. channel vs. binned tick
  unsigned int rmsWindowWtick =0;
  unsigned int rmsWindowNtick =0;
  std::map<std::string, TH1*> hrmsWindowChan;   // Histograms of RMS vs. binned tick for each channel
  std::map<std::string, TH1*> hmeanWindowChan;  // Histograms of mean vs. binned tick for each channel

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
  // Optionally returns hsameRange which is the distribution of
  // contiguous non-sticky channels with identical values.
  TH1* signalChannel(unsigned int chan,
                     TH1** pphstuckRange =nullptr,
                     TH1** pphsameRange =nullptr,
                     TH1** pphmod =nullptr);
  TH1* signstChannel(unsigned int chan);
  TH1* stuckChannel(unsigned int chan);
  TH1* sameChannel(unsigned int chan);
  TH1* modChannel(unsigned int chan);

  // Histogram of status for each channel (1=bad, 2=noisy).
  TH1* chanstat();

  // Status for each channel (1=bad, 2=noisy).
  int chanstat(unsigned int chan);

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

  // Fetch the mean signal in each channel.
  TH1* mean();

  // Fetch the RMS in each channel.
  TH1* rms();

  // Fetch the 4-sigma truncated RMS in each channel.
  TH1* rmsTruncated();

  // Fetch the not-stuck mean signal in each channel.
  TH1* meanNotSticky();

  // Fetch the 4-sigma truncated RMS in each channel.
  TH1* rmsNotSticky();

  // Fetch the 4-sigma truncated RMS in each channel.
  TH1* rmsTruncatedNotSticky();

  // Fetch the fraction of ticks in a contiguous group of hstuckthresh or more.
  TH1* stuck(int a_stuckthresh =-1);

  // Fetch the fraction of non-sticky ticks in a contiguous group of hsamethresh or more.
  TH1* same(int a_samethresh =-1);

  // Fetch the pedstal for achannel.
  double pedestal(unsigned int chan);

  // Histogram of RMS vs channel vs tick.
  //  wtick - # ticks/bin
  //  ntick - # ticks used to calculate the RMS (0 = same as wtick)
  //  pphmena - pointer to mean histogram pointer if not null
  TH2* rmsWindow(unsigned int wtick, unsigned int ntick =0, TH2** phmean =nullptr);

  // Histogram of mean vs channel vs tick.
  //  wtick - # ticks/bin
  //  ntick - # ticks used to calculate the RMS (0 = same as wtick)
  // Calls rmsWindow.
  TH2* meanWindow(unsigned int wtick, unsigned int ntick =0);

  // Return the RMS window distribution for a channel using the specified window.
  // If wtick is zero then the window from the last call to rmsWindow is used.
  TH1* rmsWindowChan(unsigned int chan, unsigned int wtick =0, unsigned int ntick =0);

  // Return the RMS window distribution for a channel using the window
  // configuration from the last call to rmsWindow.
  TH1* meanWindowChan(unsigned int chan);

};

#endif
