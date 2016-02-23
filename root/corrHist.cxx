// corrHist.cxx

// David Adams
// February 2016

// Create a correlation histogram from a 2D histogrram of channel vs. tick.

#include "TH2F.h"
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

typedef vector<double> FloatVec;
typedef vector<FloatVec> SignalVec;

TH2* corrHist(TH2* phin, int a_chanmin =0, int a_chanmax =0, int dbg =1) {
  unsigned int ntick = phin->GetNbinsX();
  unsigned int nchan = phin->GetNbinsY();
  if ( dbg ) cout << "nchan: " << nchan << endl;
  if ( dbg ) cout << "ntick: " << ntick << endl;
  float cmin = a_chanmin;
  float cmax = a_chanmax;
  if ( chanmin < 0 ) {
    float cmin = phin->GetYaxis()->GetXmin();
    float cmax = phin->GetYaxis()->GetXmax();
  }
  int
  string haxlab = phin->GetYaxis()->GetTitle();
  string hname = phin->GetName();
  hname += "corr";
  string htitle = phin->GetTitle();
  htitle += ";" + haxlab;
  htitle += ";" + haxlab;
  string::size_type ipos = htitle.find("signals");
  if ( ipos != string::npos ) {
    htitle.replace(ipos, 7, "correlations");
  } else {
    htitle = "Correlations for " + htitle;
  }
  int nchanout = cmax - cmin;
  TH2* ph = new TH2F(hname.c_str(), htitle.c_str(), nchanout, cmin, cmax, nchanout, cmin, cmax);
  ph->SetStats(0);
  ph->SetContour(40);
  ph->SetMinimum(-1.0);
  ph->SetMaximum(1.0);
  // Build the mean and RMS for each channel.
  if ( dbg ) cout << "Building means..." << endl;
  FloatVec mean(nchan, 0.0);
  FloatVec rms(nchan, 0.0);
  for ( unsigned int ichan=cmin; ichan<cmax; ++ichan ) {
    unsigned int ibin = (ichan+1)*(ntick+2);
    double sum = 0.0;
    double sumsq = 0.0;
    for ( unsigned int itick=0; itick<ntick; ++itick ) {
      ++ibin;
      double sig = phin->GetBinContent(ibin);
      sum += sig;
      sumsq += sig*sig;
    }
    mean[ichan] = sum/ntick;
    double rmssq = sumsq/ntick - mean[ichan]*mean[ichan];
    if ( rmssq > 0.0 ) rms[ichan] = sqrt(rmssq);
    if ( dbg > 1 ) {
      cout << "  mean(" << ichan << ") = " << mean[ichan]
           << " +/- " << rms[ichan] << endl;
    }
  }
  // Build the correlation for each channel pair.
  if ( dbg ) cout << "Building correlations..." << endl;
  for ( unsigned int jchan=cmin; jchan<cmax; ++jchan ) {
    for ( unsigned int ichan=cmin; ichan<=jchan; ++ichan ) {
      unsigned int jbin = (jchan+1)*(ntick+2);
      unsigned int ibin = (ichan+1)*(ntick+2);
      double sumij = 0.0;
      for ( unsigned int itick=0; itick<ntick; ++itick ) {
        ++ibin;
        ++jbin;
        double sigi = phin->GetBinContent(ibin);
        double sigj = phin->GetBinContent(jbin);
        sumij += sigi*sigj;
      }
      double num = sumij/ntick - mean[ichan]*mean[jchan];
      double den = rms[ichan]*rms[jchan];
      double corr = 0.0;
      if ( den > 0.0 ) {
        corr = num/den;
      }
      unsigned int ijbin = (jchan-cmin+1)*(nchanout+2) + ichan-cmin + 1;
      unsigned int jibin = (ichan-cmin+1)*(nchanout+2) + jchan-cmin + 1;
      ph->SetBinContent(ijbin, corr);
      ph->SetBinContent(jibin, corr);
      if ( dbg > 2 ) {
        cout << "  " << ijbin << ": corr(" << ichan << ", " << jchan << ") = " << corr << endl;
      }
    }
  }
  return ph;
}
