// FFTHist.cxx

#include "FFTHist.h"
#include <string>
#include <iostream>
#include "TH2F.h"
#include "TFFTRealComplex.h"

using std::string;

//**********************************************************************

FFTHist::FFTHist(TH2* phin, int tmin, int atmax, double ftick)
: htime0(phin), htime(0) {
  double tmax = atmax;
  if ( atmax <= tmin ) atmax = htime0->GetXaxis()->GetXmax();
  unsigned int ny = htime0->GetNbinsY();
  unsigned int nt = tmax - tmin;
  unsigned int nk = nt/2 + 1;   // Skip the conjugate frequencies
  double fmax = nk;
  string flab = "Frequency #";
  if ( ftick > 0.0 ) {
   fmax = 0.5*ftick;
   flab = "Frequency [kHz]";
  }
  // Frequency histogram.
  {
    string hname = htime0->GetName();
    hname += "_freq";
    string htitl = "FFT mag of ";
    htitl += htime0->GetTitle();
    hfreq = new TH2F(hname.c_str(), htitl.c_str(), nk, 0, fmax, ny, 0, ny);
    hfreq->GetXaxis()->SetTitle(flab.c_str());
    hfreq->GetYaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hfreq->SetStats(0);
    hfreq->SetContour(40);
    double fzmax = 100;
    hfreq->GetZaxis()->SetRangeUser(-fzmax, fzmax);
  }
  // Phase histogram.
  {
    string hname = htime0->GetName();
    hname += "_phas";
    string htitl = "FFT phase of ";
    htitl += htime0->GetTitle();
    hphase = new TH2F(hname.c_str(), htitl.c_str(), nk, 0, fmax, ny, 0, ny);
    hphase->GetXaxis()->SetTitle(flab.c_str());
    hphase->GetYaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hphase->SetStats(0);
    hphase->SetContour(40);
    double pi = acos(-1.0);
    hphase->GetZaxis()->SetRangeUser(-pi, pi);
  }
  // Time power histogram.
  {
    string hname = htime0->GetName();
    hname += "_timepower";
    string htitl = "Time power of ";
    htitl += htime0->GetTitle();
    hptime = new TH1F(hname.c_str(), htitl.c_str(), ny, 0, ny);
    hptime->GetXaxis()->SetTitle(htime0->GetYaxis()->GetTitle());
    hptime->GetYaxis()->SetTitle("Power");
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
  }
  TFFTRealComplex fft(nt, false);
  double pi = acos(-1.0);
  double twopi = 2.0*pi;
  for ( unsigned int iy=0; iy<ny; ++iy ) {
    //cout << "Initializing" << endl;
    int dummy[1] = {0};
    fft.Init("P", -1, dummy);
    double pow = 0.0;
    for ( unsigned int it=0; it<nt; ++it ) {
      unsigned int ibin = htime0->GetBin(it+tmin+1, iy+1);
      double val = htime0->GetBinContent(ibin);
      pow += val*val;
      fft.SetPoint(it, val);
    }
    hptime->SetBinContent(iy+1, pow);
    //cout << "Transform " << iy << " / " << ny << endl;
    fft.Transform();
    pow = 0.0;
    for ( unsigned int ik=0; ik<nk; ++ik ) {
      unsigned int ibin = hfreq->GetBin(ik+1, iy+1);
      double vr;
      double vi;
      fft.GetPointComplex(ik, vr, vi);
      double magsq = vr*vr + vi*vi;
      magsq /= nt;   // This normalization gives the same poser for time and freq.
      double mag = sqrt(magsq);
      double phase = atan2(vi, vr);
      double phaseoff = twopi*tmin*ik/nt;
      double phasecor = phase + phaseoff;
      while ( phasecor > pi ) phasecor -= twopi;
      //double phase = atan2(vi, vr);
      hfreq->SetBinContent(ibin, mag);
      hphase->SetBinContent(ibin, phasecor);
      pow += magsq;
      if ( ik!=0 && ik!=nk-1 ) pow += magsq;
    }
    hpfreq->SetBinContent(iy+1, pow);
  }
}

//**********************************************************************

TH2* FFTHist::maketime(int tmin, int tmax) {
  unsigned int ny = hfreq->GetNbinsY();
  unsigned int nt = tmax - tmin;
  unsigned int nk = hfreq->GetNbinsX();
  double tref0 = hfreq->GetXaxis()->GetXmin();
  double pi = acos(-1.0);
  double twopi = 2.0*pi;
  {
    string hname = htime0->GetName();
    hname += "_calc";
    string htitl = "Calculated";
    htitl += htime0->GetTitle();
    htime = new TH2F(hname.c_str(), htitl.c_str(), nt, tmin, tmax, ny, 0, ny);
    htime->SetStats(0);
    htime->SetContour(40);
    double fzmax = 100;
    htime->GetZaxis()->SetRangeUser(-fzmax, fzmax);
  }
  for ( unsigned int iy=0; iy<ny; ++iy ) {
    for ( unsigned int it=0; it<nt; ++it ) {
      double time = it + tmin;
      double sum = 0.0;
      for ( unsigned int ik=0; ik<nk; ++ik ) {
        unsigned int ibin = hfreq->GetBin(ik+1, iy+1);
        double mag = hfreq->GetBinContent(ibin);
        double pha = hphase->GetBinContent(ibin);
        unsigned int itcor = tmin + it;
        double arg = twopi*itcor*ik/nk + pha;
        double term = mag*cos(arg);
        sum += term;
        if ( ik!=0 && ik!=nk-1 ) sum += term;
      }
      unsigned int ibinout = htime->GetBin(it+1, iy+1);
      htime->SetBinContent(ibinout, sum);
    }
  }
  return htime;
}

//**********************************************************************
