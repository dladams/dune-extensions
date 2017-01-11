// DrawResult.cxx

#include "DrawResult.h"
#include "howStuck.h"
#include "TruncatedHist.h"
#include <string>
#include <iostream>
#include <sstream>
#include "TH2.h"

using std::string;
using std::cout;
using std::endl;

//**********************************************************************

DrawResult::DrawResult(int atmin, int atmax)
: tmin(atmin), tmax(atmax) {
  if ( tmax <= tmin ) tmax = tmin = 0;
}

//**********************************************************************

TH2* DrawResult::time() const {
  return hdraw;
}

//**********************************************************************

TH1* DrawResult::channels() const {
  return hdrawy;
}

//**********************************************************************

TH1* DrawResult::ticks() const {
  return hdrawx;
}

//**********************************************************************

TH1* DrawResult::timeChannel(unsigned int chan) const {
  if ( chan >= hdrawxChan.size() ) return 0;
  return hdrawxChan[chan];
}

//**********************************************************************

TH1* DrawResult::signalChannel(unsigned int chan,
                               TH1** pphstuckRange,
                               TH1** pphsameRange,
                               TH1** pphmod) {
  const string myname = "DrawResult::signalChannel: ";
  if ( chan >= hdrawxChan.size() ) return 0;
  if ( hsigChan.size() <= chan ) {
    hsigChan.resize(chan+1, nullptr);
    hsinChan.resize(chan+1, nullptr);
    hstuckRange.resize(chan+1, nullptr);
    hsameRange.resize(chan+1, nullptr);
    hmodChan.resize(chan+1, nullptr);
  }
  TH1*& phsig = hsigChan[chan];
  TH1*& phsin = hsinChan[chan];
  TH1*& phstuckRange = hstuckRange[chan];
  TH1*& phsameRange = hsameRange[chan];
  TH1*& phmod = hmodChan[chan];
  int maxStuck = 50;
  if ( phsig == nullptr ) {
    TH1* phtim = hdrawxChan[chan];
    if ( phtim == nullptr ) {
      cout << myname << "Time spectrum not found for channel " << chan << endl;
      return nullptr;
    }
    string hname = string(phtim->GetName()) + "_signal";
    string hbname = string(phtim->GetName()) + "_signst";
    string hsname = string(phtim->GetName()) + "_stuckrange";
    string hrname = string(phtim->GetName()) + "_samerange";
    string hmname = string(phtim->GetName()) + "_mod64";
    string htitl = "Signal for " + string(phtim->GetTitle());
    string hbtitl = "Not-sticky signal for " + string(phtim->GetTitle());
    string hstitl = "Stuck-bit ranges for " + string(phtim->GetTitle());
    string hrtitl = "Same-value ranges for " + string(phtim->GetTitle());
    string hmtitl = "Mod64 " + string(phtim->GetTitle());
    int nbin = nsig;
    double xmin = sigmin;
    double xmax = sigmax;
    if ( nbin == -1 ) {
      double dsig = sigmin;
      if ( dsig <= 0.0 ) dsig = 1.0;
      xmin = phtim->GetMinimum();
      xmax = phtim->GetMaximum() + dsig;
      nbin = (xmax - xmin)/dsig;
    }
    phsig = new TH1F(hname.c_str(), htitl.c_str(), nbin, xmin, xmax);
    phsig->GetXaxis()->SetTitle(phtim->GetYaxis()->GetTitle());
    phsig->GetYaxis()->SetTitle("Count");
    phsin = new TH1F(hbname.c_str(), hbtitl.c_str(), nbin, xmin, xmax);
    phsin->GetXaxis()->SetTitle(phtim->GetYaxis()->GetTitle());
    phsin->GetYaxis()->SetTitle("Count");
    phstuckRange = new TH1F(hsname.c_str(), hstitl.c_str(), maxStuck, 0, maxStuck);
    phstuckRange->GetXaxis()->SetTitle("# contiguous ticks with stuck bits");
    phstuckRange->GetYaxis()->SetTitle("Count");
    phsameRange = new TH1F(hrname.c_str(), hrtitl.c_str(), maxStuck, 0, maxStuck);
    phsameRange->GetXaxis()->SetTitle("# contiguous ticks with same value");
    phsameRange->GetYaxis()->SetTitle("Count");
    phmod = new TH1F(hmname.c_str(), hmtitl.c_str(), 64, 0, 64);
    string slab = "mod64(" + string(phtim->GetYaxis()->GetTitle()) + ")";
    phmod->GetXaxis()->SetTitle(phtim->GetYaxis()->GetTitle());
    phmod->GetYaxis()->SetTitle("Count");
    int nstuck = 0;
    bool isStuck = false;
    int ntbin = phtim->GetNbinsX();
    double xadcLast = -1.e20;
    unsigned int nsame = 0;
    for ( int ibin=1; ibin<=ntbin; ++ibin ) {
      bool isLast = ibin == ntbin;
      double xadc = phtim->GetBinContent(ibin);
      phsig->Fill(xadc);
      unsigned short iadc = havePedestal ? xadc + pedestal(chan) : xadc + 0.001;
      phmod->Fill(iadc%64);
      bool wasStuck = isStuck;
      if ( ! wasStuck ) nstuck = 0;
      isStuck = howStuck(iadc);
      if ( ! isStuck ) phsin->Fill(xadc);
      if ( ! isStuck ) phstuckRange->Fill(-1);
      if ( isStuck ) ++nstuck;    // # consecutive sticks
      if ( wasStuck ) {
        if ( ! isStuck || isLast ) {
          phstuckRange->Fill(nstuck, nstuck);
        }
      }
      bool isSame = xadc == xadcLast;
      if ( isStuck ) isSame = false;
      // If value has changed, fill for the last bin.
      if ( nsame > 0 ) {
        if ( !isSame ) {
          for ( unsigned int ifil=0; ifil<nsame; ++ifil ) phsameRange->Fill(nsame);
        }
      }
      nsame = isSame ? nsame+1 : 1;
      // If this is the last, fill for this bin.
      if ( isLast ) phsameRange->Fill(nsame, nsame);
      xadcLast = xadc;
    } 
  }
  if ( pphstuckRange != nullptr ) *pphstuckRange = phstuckRange;
  if ( pphsameRange != nullptr ) *pphsameRange = phsameRange;
  if ( pphmod != nullptr ) *pphmod = phmod;
  return phsig;
}

//**********************************************************************

TH1* DrawResult::stuckChannel(unsigned int chan) {
  TH1* ph = nullptr;
  signalChannel(chan, &ph, nullptr, nullptr);
  return ph;
}

//**********************************************************************

TH1* DrawResult::sameChannel(unsigned int chan) {
  TH1* ph = nullptr;
  signalChannel(chan, nullptr, &ph, nullptr);
  return ph;
}

//**********************************************************************

TH1* DrawResult::modChannel(unsigned int chan) {
  TH1* ph = nullptr;
  signalChannel(chan, nullptr, nullptr, &ph);
  if ( ph != nullptr ) ph->SetStats(0);
  return ph;
}

//**********************************************************************

TH1* DrawResult::signstChannel(unsigned int chan) {
  signalChannel(chan, nullptr, nullptr, nullptr);
  TH1* ph = hsinChan.size() > chan ? hsinChan[chan] : nullptr;
  if ( ph != nullptr ) ph->SetStats(0);
  return ph;
}

//**********************************************************************

TH1* DrawResult::chanstat() {
  return hchanstat;
}

//**********************************************************************

int DrawResult::chanstat(unsigned int chan) {
  return chanstats.size() > chan ? chanstats[chan] : -1;
}

//**********************************************************************

TH2* DrawResult::freq() {
  const string myname = "DrawResult::freq: ";
  if ( pfft == nullptr ) {
    if ( hdraw == nullptr ) {
      cout << myname << "ERROR: Unable to find FFT or time histogram." << endl;
      return nullptr;
    }
    cout << myname << "Performing FFT." << endl;
    pfft = new FFTHist(hdraw, tmin, tmax, 2000);
  }
  return pfft->hfreq;
}

//**********************************************************************

TH2* DrawResult::power() {
  const string myname = "DrawResult::power: ";
  if ( freq() == nullptr ) return nullptr;
  return pfft->hpower;
}

//**********************************************************************

TH1* DrawResult::freqChannel(unsigned int chan) {
  const string myname = "DrawResult::freqChannel: ";
  TH2* phfreq = freq();
  if ( phfreq == nullptr ) {
    cout << myname << "ERROR: Unable to find FFT spectrum." << endl;
    return nullptr;
  }
  unsigned int nchan = phfreq->GetNbinsY();
  if ( chan >= nchan ) {
    cout << myname << "Invalid channel number: " << chan << endl;
    return nullptr;
  }
  if ( hfreqChan.size() < nchan ) hfreqChan.resize(nchan, nullptr);
  TH1*& ph = hfreqChan[chan];
  if ( ph == nullptr ) {
    ostringstream sshname;
    sshname << phfreq->GetName() << "_chan";
    if ( chan < 100 ) sshname << "0";
    if ( chan < 10 ) sshname << "0";
    sshname << chan;
    string hname = sshname.str();
    ph = phfreq->ProjectionX(hname.c_str(), chan+1, chan+1);
    ph->SetStats(0);
    ostringstream sshtitle;
    sshtitle << ph->GetTitle() << " channel " << chan;
    ph->SetTitle(sshtitle.str().c_str());
  }
  return ph;
}

//**********************************************************************

TH1* DrawResult::powerChannel(unsigned int chan) {
  const string myname = "DrawResult::powerChannel: ";
  TH2* phfpwr = power();
  if ( phfpwr == nullptr ) {
    cout << myname << "ERROR: Unable to find FFT spectrum." << endl;
    return nullptr;
  }
  unsigned int nchan = phfpwr->GetNbinsY();
  if ( chan >= nchan ) {
    cout << myname << "Invalid channel number: " << chan << endl;
    return nullptr;
  }
  if ( hfpwrChan.size() < nchan ) hfpwrChan.resize(nchan, nullptr);
  TH1*& ph = hfpwrChan[chan];
  if ( ph == nullptr ) {
    ostringstream sshname;
    sshname << phfpwr->GetName() << "_chan";
    if ( chan < 100 ) sshname << "0";
    if ( chan < 10 ) sshname << "0";
    sshname << chan;
    string hname = sshname.str();
    ph = phfpwr->ProjectionX(hname.c_str(), chan+1, chan+1);
    ph->SetStats(0);
    ostringstream sshtitle;
    sshtitle << ph->GetTitle() << " channel " << chan;
    ph->SetTitle(sshtitle.str().c_str());
  }
  return ph;
}

//**********************************************************************

TH1* DrawResult::timePowerChannel(unsigned int chan) {
  const string myname = "DrawResult::powerChannel: ";
  TH2* phtpwr = time();
  if ( phtpwr == nullptr ) {
    cout << myname << "ERROR: Unable to find time spectrum." << endl;
    return nullptr;
  }
  unsigned int nchan = phtpwr->GetNbinsY();
  if ( chan >= nchan ) {
    cout << myname << "Invalid channel number: " << chan << endl;
    return nullptr;
  }
  if ( htpwrChan.size() < nchan ) htpwrChan.resize(nchan, nullptr);
  TH1*& ph = htpwrChan[chan];
  if ( ph == nullptr ) {
    ostringstream sshname;
    sshname << phtpwr->GetName() << "_chan";
    if ( chan < 100 ) sshname << "0";
    if ( chan < 10 ) sshname << "0";
    sshname << chan;
    string hname = sshname.str();
    TH1* pht = timeChannel(chan);
    ph = new TH1F(hname.c_str(), pht->GetTitle(), tmax-tmin, tmin, tmax);
    for ( int ibin=1; ibin<=ph->GetNbinsX(); ++ibin ) {
      double sig = ph->GetBinContent(ibin);
      ph->SetBinContent(ibin, sig*sig);
    }
    ph->SetStats(0);
    ostringstream sshtitle;
    sshtitle << ph->GetTitle() << " channel " << chan;
    ph->SetTitle(sshtitle.str().c_str());
  }
  return ph;
}


//**********************************************************************

TH1* DrawResult::mean() {
  const string myname = "DrawResult::mean: ";
  if ( hmean == nullptr ) {
    if ( hdraw == nullptr ) return nullptr;
    unsigned int ncha = hdrawxChan.size();
    string hname = string(hdraw->GetName()) + "_mean";
    string htitl = string(hdraw->GetTitle()) + " mean ADC count";
    hmean = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hmean->GetXaxis()->SetTitle("Channel");
    hmean->GetYaxis()->SetTitle("Mean [ADC counts]");
    hmean->SetStats(0);
    hmean->SetMinimum(limmeanlo);
    hmean->SetMaximum(limmeanhi);
    hname = string(hdraw->GetName()) + "_meantrunc";
    htitl = string(hdraw->GetTitle()) + " truncated mean ADC count";
    hmet = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hmet->GetXaxis()->SetTitle("Channel");
    hmet->GetYaxis()->SetTitle("Mean [ADC counts]");
    hmet->SetStats(0);
    hmet->SetMinimum(limmeanlo);
    hmet->SetMaximum(limmeanhi);
    hname = string(hdraw->GetName()) + "_rms";
    htitl = string(hdraw->GetTitle());
    string::size_type ipos = htitl.find("signals");
    if ( ipos != string::npos ) {
      htitl.replace(ipos+6, 1, " RMS");
    } else {
      htitl += ": RMS";
    }
    hrms = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hrms->GetXaxis()->SetTitle("Channel");
    hrms->GetYaxis()->SetTitle("RMS [ADC counts]");
    hrms->SetStats(0);
    hrms->SetMinimum(0.0);
    hrms->SetMaximum(50.0);
    hrmt = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hrmt->GetXaxis()->SetTitle("Channel");
    hrmt->GetYaxis()->SetTitle("Truncated RMS [ADC counts]");
    hrmt->SetStats(0);
    hrmt->SetMinimum(0.0);
    hrmt->SetMaximum(50.0);
    for ( unsigned int icha=0; icha<hdrawxChan.size(); ++icha ) {
      TH1* ph = signalChannel(icha);
      double mean = ph->GetMean();
      double rms = ph->GetRMS();
      hmean->SetBinContent(icha+1, mean);
      hrms->SetBinContent(icha+1, rms);
      TruncatedHist th(ph, truncsigma);
      double rmt = th.hist()->GetRMS();
      hrmt->SetBinContent(icha+1, rmt);
    }
  }
  return hmean;
}

//**********************************************************************

TH1* DrawResult::rms() {
  if ( hrms == nullptr ) mean();
  return hrms;
}

//**********************************************************************

TH1* DrawResult::meanTruncated() {
  if ( hmet == nullptr ) mean();
  return hrmt;
}

//**********************************************************************

TH1* DrawResult::rmsTruncated() {
  if ( hrmt == nullptr ) mean();
  return hrmt;
}

//**********************************************************************

TH1* DrawResult::meanNotSticky() {
  const string myname = "DrawResult::meanNotSticky: ";
  if ( hmen == nullptr ) {
    if ( hdraw == nullptr ) return nullptr;
    unsigned int ncha = hdrawxChan.size();
    string hname = string(hdraw->GetName()) + "_meanns";
    string htitl = string(hdraw->GetTitle()) + " mean not-sticky ADC count";
    hmen = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hmen->GetXaxis()->SetTitle("Channel");
    hmen->GetYaxis()->SetTitle("Mean [ADC counts]");
    hmen->SetMinimum(limmeanlo);
    hmen->SetMaximum(limmeanhi);
    hmen->SetStats(0);
    ostringstream ssnsig;
    ssnsig << truncsigma;
    string snsig = ssnsig.str();
    for ( unsigned int ich=0; ich<snsig.size(); ++ich ) {
      if ( snsig[ich] == '.' ) snsig[ich] = 'p';
    }
    hname = string(hdraw->GetName()) + "_rmsns" + snsig;
    htitl = string(hdraw->GetTitle());
    string::size_type ipos = htitl.find("signals");
    if ( ipos != string::npos ) {
      htitl.replace(ipos+6, 1, " not-sticky RMS");
    } else {
      htitl += ": not-sticky RMS";
    }
    hrmn = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hrmn->GetXaxis()->SetTitle("Channel");
    hrmn->GetYaxis()->SetTitle("RMS [ADC counts]");
    hrmn->SetStats(0);
    hrmn->SetMinimum(0.0);
    hrmn->SetMaximum(50.0);
    hmnt = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hmnt->GetXaxis()->SetTitle("Channel");
    hmnt->GetYaxis()->SetTitle("Truncated RMS [ADC counts]");
    hmnt->SetStats(0);
    hmnt->SetMinimum(limmeanlo);
    hmnt->SetMaximum(limmeanhi);
    hrnt = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    hrnt->GetXaxis()->SetTitle("Channel");
    hrnt->GetYaxis()->SetTitle("Truncated RMS [ADC counts]");
    hrnt->SetStats(0);
    hrnt->SetMinimum(0.0);
    hrnt->SetMaximum(50.0);
    for ( unsigned int icha=0; icha<hdrawxChan.size(); ++icha ) {
      TH1* ph = signstChannel(icha);
      double mean = ph->GetMean();
      double rms = ph->GetRMS();
      hmen->SetBinContent(icha+1, mean);
      hrmn->SetBinContent(icha+1, rms);
      TruncatedHist th(ph, truncsigma);
      double met = th.hist()->GetMean();
      double rmt = th.hist()->GetRMS();
      hmnt->SetBinContent(icha+1, met);
      hrnt->SetBinContent(icha+1, rmt);
    }
  }
  return hmen;
}

//**********************************************************************

TH1* DrawResult::rmsNotSticky() {
  if ( hrmn == nullptr ) meanNotSticky();
  return hrmn;
}

//**********************************************************************

TH1* DrawResult::meanTruncatedNotSticky() {
  if ( hmnt == nullptr ) meanNotSticky();
  return hmnt;
}
//**********************************************************************

TH1* DrawResult::rmsTruncatedNotSticky() {
  if ( hrnt == nullptr ) meanNotSticky();
  return hrnt;
}
//**********************************************************************

TH1* DrawResult::stuck(int a_stuckthresh) {
  const string myname = "DrawResult::stuck: ";
  if ( a_stuckthresh >= 0 && a_stuckthresh != stuckthresh ) {
    stuckthresh = a_stuckthresh;
  }
  if ( stuckthresh < 0 ) return nullptr;
  unsigned int nthresh = stuckthresh + 1;
  if ( nthresh > hstuckThresh.size() ) hstuckThresh.resize(nthresh+1, nullptr);
  TH1*& phstuck = hstuckThresh[stuckthresh];
  if ( phstuck == nullptr ) {
    if ( hdraw == nullptr ) return nullptr;
    unsigned int ncha = hdrawxChan.size();
    ostringstream ssname;
    ssname << string(hdraw->GetName()) + "_stuck" << stuckthresh;
    string hname = ssname.str();
    ostringstream sstitl;
    sstitl << hdraw->GetTitle() << " stuck range " << stuckthresh;
    string htitl = sstitl.str();
    phstuck = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    phstuck->GetXaxis()->SetTitle("Channel");
    phstuck->GetYaxis()->SetTitle("Fraction");
    phstuck->SetMinimum(0.0);
    phstuck->SetMaximum(1.0);
    phstuck->SetStats(0);
    for ( unsigned int icha=0; icha<hdrawxChan.size(); ++icha ) {
      TH1* ph = stuckChannel(icha);
      unsigned int nbin = ph->GetNbinsX();
      double num = ph->Integral(stuckthresh+1, nbin+1);
      double den = ph->Integral(0, nbin+1);
      double frac = den>0 ? num/den : -1.0;
      phstuck->SetBinContent(icha+1, frac);
    }
  }
  return phstuck;
}

//**********************************************************************

TH1* DrawResult::same(int a_samethresh) {
  const string myname = "DrawResult::mean: ";
  if ( a_samethresh >= 0 && a_samethresh != samethresh ) {
    samethresh = a_samethresh;
  }
  if ( samethresh < 0 ) return nullptr;
  unsigned int nthresh = samethresh + 1;
  if ( nthresh > hsameThresh.size() ) hsameThresh.resize(nthresh+1, nullptr);
  TH1*& phsame = hsameThresh[samethresh];
  if ( phsame == nullptr ) {
    if ( hdraw == nullptr ) return nullptr;
    unsigned int ncha = hdrawxChan.size();
    ostringstream ssname;
    ssname << string(hdraw->GetName()) + "_same" << samethresh;
    string hname = ssname.str();
    ostringstream sstitl;
    sstitl << hdraw->GetTitle() << " same range " << samethresh;
    string htitl = sstitl.str();
    phsame = new TH1F(hname.c_str(), htitl.c_str(), ncha, 0, ncha);
    phsame->GetXaxis()->SetTitle("Channel");
    phsame->GetYaxis()->SetTitle("Fraction");
    phsame->SetMinimum(0.0);
    phsame->SetMaximum(1.0);
    phsame->SetStats(0);
    for ( unsigned int icha=0; icha<hdrawxChan.size(); ++icha ) {
      TH1* ph = sameChannel(icha);
      unsigned int nbin = ph->GetNbinsX();
      double num = ph->Integral(samethresh+1, nbin+1);
      double den = ph->Integral(0, nbin+1);
      double frac = den>0 ? num/den : -1.0;
      phsame->SetBinContent(icha+1, frac);
    }
  }
  return phsame;
}

//**********************************************************************

double DrawResult::pedestal(unsigned int chan) {
  const string myname = "DrawResult::pedestal: ";
  if ( pedestals.size() > chan ) return pedestals[chan];
  return 0.0;
}

//**********************************************************************

TH2* DrawResult::rmsWindow(unsigned int a_wtick, unsigned int a_ntick, TH2** pphmean) {
  int wtick = a_wtick ? a_wtick : 0;
  int ntick = a_ntick ? a_ntick : wtick;
  rmsWindowWtick = wtick;
  rmsWindowNtick = ntick;
  if ( wtick == 0 ) return nullptr;
  TH2* phsig = time();
  ostringstream sshname;
  sshname << phsig->GetName() << "_rmsw_" << wtick << "_" << ntick;
  string hname = sshname.str();
  ostringstream sshnameMean;
  sshnameMean << phsig->GetName() << "_meanw_" << wtick << "_" << ntick;
  string hnameMean = sshnameMean.str();
  string htitl = "Binned RMS for " + string(phsig->GetTitle());
  string htitlMean = "Binned mean for " + string(phsig->GetTitle());
  TH2*& phrms = hrmsWindow[hname];
  TH2*& phmea = hmeanWindow[hname];
  if ( pphmean != nullptr ) *pphmean = phmea;
  if ( phrms != nullptr ) return phrms;
  int nbinsig = phsig->GetNbinsX();
  int nbin = nbinsig/wtick;
  int nchan = phsig->GetNbinsY();
  if ( nbin <= 0 || nchan <= 0 ) return nullptr;
  double xmin = phsig->GetXaxis()->GetXmin();
  double xmax = xmin + nbin*wtick;
  double ymin = phsig->GetYaxis()->GetXmin();
  double ymax = phsig->GetYaxis()->GetXmax();
  unsigned int chan0 = 0;
  int lotick = (wtick - ntick)/2;
  int hitick = lotick + ntick;
  phrms = new TH2F(hname.c_str(), htitl.c_str(), nbin, xmin, xmax, nchan, ymin, ymax);
  phrms->SetStats(0);
  phrms->SetMinimum(0.0);
  phrms->SetMaximum(100.0);
  phmea = new TH2F(hnameMean.c_str(), htitlMean.c_str(), nbin, xmin, xmax, nchan, ymin, ymax);
  phmea->SetStats(0);
  phmea->SetMinimum(-100.0);
  phmea->SetMaximum( 100.0);
  for ( int iy=0; iy<nchan; ++iy ) {
    for ( int ix=0; ix<nbin; ++ix ) {
      int binout = phrms->GetBin(ix+1, iy+1);
      int tick0 = xmin + ix*wtick;
      int tick1 = tick0 + lotick;
      if ( tick1 < 0 ) tick1 = 0;
      int tick2 = tick0 + hitick;
      if ( tick2 > nbinsig ) tick2 = nbinsig;
      double ntick = tick2 - tick1;
      double sum = 0.0;
      double sumsq = 0.0;
      for ( int tick=tick1; tick<tick2; ++tick ) {
        int bin = phsig->GetBin(tick+1, iy+1);
        double sig = phsig->GetBinContent(bin);
        sum += sig;
        sumsq += sig*sig;
      }
      double mean = sum/ntick;
      double rmssq = sumsq/ntick - mean*mean;
      double rms = sqrt(rmssq);
      phrms->SetBinContent(binout, rms);
      phmea->SetBinContent(binout, mean);
    }
  }
  return phrms;
}
  
//**********************************************************************

TH2* DrawResult::meanWindow(unsigned int a_wtick, unsigned int a_ntick) {
  TH2* ph = nullptr;
  rmsWindow(a_wtick, a_ntick, &ph);
  return ph;
}

//**********************************************************************

TH1* DrawResult::rmsWindowChan(unsigned int chan, unsigned int a_wtick, unsigned int a_ntick) {
  unsigned int wtick = a_wtick;
  unsigned int ntick = a_ntick;
  if ( wtick == 0 ) {
    wtick = rmsWindowWtick;
    ntick = rmsWindowNtick;
  }
  TH2* phrms = rmsWindow(wtick, ntick);
  ostringstream sshname;
  sshname << phrms->GetName() << "_chan" << chan;
  string hname = sshname.str();
  TH1*& ph = hrmsWindowChan[hname];
  if ( ph != nullptr ) return ph;
  int bin = chan + 1;
  ph = phrms->ProjectionX(hname.c_str(), bin, bin);
  ph->SetStats(0);
  ph->SetMinimum(0.0);
  ph->SetMaximum(100.0);
  return ph;
}

//**********************************************************************

TH1* DrawResult::meanWindowChan(unsigned int chan) {
  TH2* phrms = meanWindow(rmsWindowWtick, rmsWindowNtick);
  ostringstream sshname;
  sshname << phrms->GetName() << "_chan" << chan;
  string hname = sshname.str();
  TH1*& ph = hmeanWindowChan[hname];
  if ( ph != nullptr ) return ph;
  int bin = chan + 1;
  ph = phrms->ProjectionX(hname.c_str(), bin, bin);
  ph->SetStats(0);
  ph->SetMinimum(-100.0);
  ph->SetMaximum( 100.0);
  return ph;
}

//**********************************************************************
