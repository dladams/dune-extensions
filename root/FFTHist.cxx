// FFTHist.cxx

#include "FFTHist.h"
#include <string>
#include <iostream>
#include <sstream>
#include "TH2F.h"
#include "TFFTRealComplex.h"

using std::string;
using std::ostringstream;

//**********************************************************************

FFTHist::FFTHist(TH2* phin, int atmin, int atmax, double ftick)
: phase0(true), tmin(atmin), tmax(atmax), htime0(phin), htime(0) {
  if ( tmax <= tmin ) {
    tmin = htime0->GetXaxis()->GetXmin();
    tmax = htime0->GetXaxis()->GetXmax();
  }
  unsigned int nc = htime0->GetNbinsY();
  unsigned int nt = tmax - tmin;
  unsigned int nk = nt/2 + 1;   // Skip the conjugate frequencies
  bool ntEven = 2*(nt/2) == nt;
  double kmax = nk;
  string flab = "Frequency #";
  if ( ftick > 0.0 ) {
   kmax = 0.5*ftick;
   flab = "Frequency [kHz]";
  }
  // Frequency histogram.
  {
    ostringstream sshname;
    sshname << htime0->GetName() << "_" << tmin << "_" << tmax;
    string hname = sshname.str();
    hname += "_freq";
    string htitl = "FFT mag of ";
    htitl += htime0->GetTitle();
    hfreq = new TH2F(hname.c_str(), htitl.c_str(), nk, 0, kmax, nc, 0, nc);
    hfreq->GetXaxis()->SetTitle(flab.c_str());
    hfreq->GetYaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hfreq->SetStats(0);
    hfreq->SetContour(20);
    hfreq->SetMinimum(0.0);
    double fzmax = 100;
    hfreq->GetZaxis()->SetRangeUser(-fzmax, fzmax);
    hfreq->SetStats(0);
  }
  // Power histogram.
  {
    ostringstream sshname;
    sshname << htime0->GetName() << "_" << tmin << "_" << tmax;
    string hname = sshname.str();
    hname += "_power";
    string htitl = "FFT power of ";
    htitl += htime0->GetTitle();
    hpower = new TH2F(hname.c_str(), htitl.c_str(), nk, 0, kmax, nc, 0, nc);
    hpower->GetXaxis()->SetTitle(flab.c_str());
    hpower->GetYaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hpower->SetStats(0);
    hpower->SetContour(20);
    hpower->SetMinimum(0.0);
    double fzmax = 100;
    hpower->GetZaxis()->SetRangeUser(-fzmax, fzmax);
    hpower->SetStats(0);
  }
  // Phase histogram.
  {
    string hname = htime0->GetName();
    hname += "_phas";
    string htitl = "FFT phase of ";
    htitl += htime0->GetTitle();
    hphase = new TH2F(hname.c_str(), htitl.c_str(), nk, 0, kmax, nc, 0, nc);
    hphase->GetXaxis()->SetTitle(flab.c_str());
    hphase->GetYaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hphase->SetStats(0);
    hphase->SetContour(40);
    double pi = acos(-1.0);
    hphase->GetZaxis()->SetRangeUser(-pi, pi);
    hphase->SetStats(0);
  }
  // Time power histogram.
  {
    string hname = htime0->GetName();
    hname += "_timepower";
    string htitl = "Time power of ";
    htitl += htime0->GetTitle();
    hptime = new TH1F(hname.c_str(), htitl.c_str(), nc, 0, nc);
    hptime->GetXaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hptime->GetYaxis()->SetTitle("Power");
    hptime->SetStats(0);
  }
  // Frequency power histogram.
  {
    string hname = htime0->GetName();
    hname += "_freqpower";
    string htitl = "Frequency power of ";
    htitl += htime0->GetTitle();
    hpfreq = new TH1F(hname.c_str(), htitl.c_str(), nk, 0, nk);
    hpfreq->GetXaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hpfreq->GetYaxis()->SetTitle("Power");
    hpfreq->SetStats(0);
  }
  double pi = acos(-1.0);
  double twopi = 2.0*pi;
  int show = 0;
  for ( unsigned int ic=0; ic<nc; ++ic ) {
    TFFTRealComplex* pfft = new TFFTRealComplex(nt, false);
    if ( show ) cout << "FFT: @" << pfft << endl;
    ffts.push_back(pfft);
    //cout << "Initializing" << endl;
    int dummy[1] = {0};
    pfft->Init("P", -1, dummy);
    double pow = 0.0;
    for ( unsigned int it=0; it<nt; ++it ) {
      unsigned int ibin = htime0->GetBin(it+tmin+1, ic+1);
      double val = htime0->GetBinContent(ibin);
      pow += val*val;
      pfft->SetPoint(it, val);
    }
    hptime->SetBinContent(ic+1, pow);
    //cout << "Transform " << ic << " / " << nc << endl;
    pfft->Transform();
    pow = 0.0;
    for ( unsigned int ik=0; ik<nk; ++ik ) {
      bool isConjugate = ik != 0;
      if ( ik==nk-1 && ntEven ) isConjugate = true;
      unsigned int ibin = hfreq->GetBin(ik+1, ic+1);
      double vr;
      double vi;
      pfft->GetPointComplex(ik, vr, vi);
      double magsq = vr*vr + vi*vi;
      magsq /= nt;   // This normalization gives the same power for time and freq.
      double mag = sqrt(magsq);
      double phase = atan2(vi, vr);
      if ( phase0 ) {
        double phaseoff = twopi*tmin*ik/nt;
        phase += phaseoff;
        while ( phase > pi ) phase -= twopi;
      }
      hfreq->SetBinContent(ibin, mag);
      double power = mag*mag;
      if ( isConjugate ) power += power;
      hpower->SetBinContent(ibin, power);
      hphase->SetBinContent(ibin, phase);
      pow += magsq;
      if ( ik!=0 && ik!=nk-1 ) pow += magsq;
      if ( show > 0 ) {
        cout << "ibin=" << ibin << ", mag=" << mag << "(" << hfreq->GetBinContent(ibin) << ")"
             << ", phase=" << phase
             << ", X= (" << vr << ", " << vi << ")"
             << endl;
        --show;
      }
    }
    hpfreq->SetBinContent(ic+1, pow);
  }
}

//**********************************************************************

TH2* FFTHist::maketime(int atmin, int atmax) {
  unsigned int nc = hfreq->GetNbinsY();
  unsigned int ntin = tmax - tmin;
  unsigned int ntout = atmax - atmin;
  unsigned int nk = hfreq->GetNbinsX();
  double tref0 = hfreq->GetXaxis()->GetXmin();
  double pi = acos(-1.0);
  double twopi = 2.0*pi;
  {
    string hname = htime0->GetName();
    hname += "_calc";
    string htitl = "Calculated";
    htitl += htime0->GetTitle();
    htime = new TH2F(hname.c_str(), htitl.c_str(), ntout, atmin, atmax, nc, 0, nc);
    htime->SetStats(0);
    htime->SetContour(40);
    double fzmax = 100;
    htime->GetZaxis()->SetRangeUser(-fzmax, fzmax);
    htime->GetZaxis()->SetTitle(htime0->GetZaxis()->GetTitle());
    htime->GetYaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    htime->GetXaxis()->SetTitle(htime0->GetXaxis()->GetTitle());
  }
  int check = 0;
  double fac = sqrt(1.0/ntin);
  for ( unsigned int ic=0; ic<nc; ++ic ) {
    TFFTRealComplex* pfft = ffts[ic];
    if ( check ) cout << "FFT: @" << pfft << endl;
    for ( unsigned int it=0; it<ntout; ++it ) {
      double sum = 0.0;
      for ( unsigned int ik=0; ik<nk; ++ik ) {
        unsigned int ibin = hfreq->GetBin(ik+1, ic+1);
        double mag = hfreq->GetBinContent(ibin);
        double pha = hphase->GetBinContent(ibin);
        if ( check ) {
          double vr, vi;
          pfft->GetPointComplex(ik, vr, vi);
          double magref = sqrt((vr*vr + vi*vi)/ntin);
          double pharef = atan2(vi, vr);
          cout << "        Bin: " << ibin << endl;
          cout << "          X: (" << vr << ", " << vi << ")" << endl;
          cout << "  Magnitude: " << mag << " != " << magref << endl;
          cout << "      Phase: " << pha << " != " << pharef << endl;
          --check;
        }
        unsigned int itcor = it + atmin;
        if ( ! phase0 ) itcor -= tmin;
        double arg = twopi*itcor*ik/ntin + pha;
        double term = fac*mag*cos(arg);
        sum += term;
        if ( ik!=0 && ik!=nk-1 ) sum += term;
      }
      unsigned int ibinout = htime->GetBin(it+1, ic+1);
      htime->SetBinContent(ibinout, sum);
    }
  }
  return htime;
}

//**********************************************************************
