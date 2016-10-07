// DrawResult.cxx

#include "DrawResult.h"
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

TH1* DrawResult::signalChannel(unsigned int chan) {
  const string myname = "DrawResult::signalChannel: ";
  if ( chan >= hdrawxChan.size() ) return 0;
  if ( hsigChan.size() <= chan ) hsigChan.resize(chan+1, nullptr);
  TH1*& phsig = hsigChan[chan];
  if ( phsig == nullptr ) {
    TH1* phtim = hdrawxChan[chan];
    if ( phtim == nullptr ) {
      cout << myname << "Time spectrum not found for channel " << chan << endl;
      return nullptr;
    }
    string hname = string(phtim->GetName()) + "_signal";
    string htitl = "Signal for " + string(phtim->GetName());
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
    for ( int ibin=1; ibin<=phtim->GetNbinsX(); ++ibin ) {
      phsig->Fill(phtim->GetBinContent(ibin));
    } 
  }
  return phsig;
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
    ph = phfreq->ProjectionX(hname.c_str(), chan, chan);
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
    ph = phfpwr->ProjectionX(hname.c_str(), chan, chan);
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
