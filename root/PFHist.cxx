// PFHist.cxx

#include "PFHist.h"
#include <string>
#include <vector>
#include "TH2F.h"

using std::string;

//**********************************************************************

PFHist::PFHist(TH2* ahin) : hin(ahin) {
  string hname = hin->GetName();
  hname += "_pf";
  string htitl = "Power fraction for ";
  htitl += hin->GetTitle();
  unsigned int nx = hin->GetNbinsX();
  unsigned int ny = hin->GetNbinsY();
  double xmin = hin->GetXaxis()->GetXmin();
  double xmax = hin->GetXaxis()->GetXmax();
  double ymin = hin->GetYaxis()->GetXmin();
  double ymax = hin->GetYaxis()->GetXmax();
  hout = new TH2F(hname.c_str(), htitl.c_str(), nx, xmin, xmax, ny, ymin, ymax);
  hout->GetXaxis()->SetTitle(hin->GetXaxis()->GetTitle());
  hout->GetYaxis()->SetTitle(hin->GetYaxis()->GetTitle());
  hout->GetXaxis()->SetTitle("Power fraction");
  hout->SetStats(0);
  hout->SetContour(40);
  double fzmax = 0.20;
  hout->GetZaxis()->SetRangeUser(-fzmax, fzmax);
  for ( unsigned int iy=0; iy<ny; ++iy ) {
    double powsum = 0.0;
    vector<double> pows(nx, 0.0);
    for ( unsigned int ix=0; ix<nx; ++ix ) {
      unsigned int ibin = hin->GetBin(ix+1, iy+1);
      double val = hin->GetBinContent(ibin);
      double pow = val*val;
      pows[ix] = pow;
      powsum += pow;
    }
    for ( unsigned int ix=0; ix<nx; ++ix ) {
      unsigned int ibin = hin->GetBin(ix+1, iy+1);
      double pf = pows[ix]/powsum;
      hout->SetBinContent(ibin, pf);
    }
  }
}

//**********************************************************************
