// draw1d.cxx
//
// Draw 1 1D histogram.

#include <string>
#include <iostream>
#include <sstream>
#include "TH1.h"
#include "TCanvas.h"
#include "TLine.h"
#include "dxhist.h"
#include "dxlabel.h"
#include "dxprint.h"

using std::string;
using std::cout;
using std::endl;
using std::ostringstream;

int draw1d(TH1* ph, float xmin=0.0, float xmax=0.0, float ymin=0.0, float ymax=0.0, int rebin=0);

int draw1d(TH1* phin, float xmin, float xmax, float ymin, float ymax, int rebin) {
  // Assign unique name for histogram.
  static int hcount = 0;
  ++hcount;
  ostringstream sshname;
  sshname << "hdraw1d" << hcount;
  string hname = sshname.str();
  cout << "Creating histogram " << hname << endl;
  TH1* ph = dynamic_cast<TH1*>(phin->Clone(hname.c_str()));
  // Construct printing label.
  TLatex* plab = dxlabel();
  if ( plab != 0 ) {
    ostringstream sslab;
    string sep = "_";
    sslab << plab->GetTitle() << sep << dxhist() << sep;
    string namein = phin->GetName();
    string::size_type iposx = namein.rfind("x");
    string::size_type iposy = namein.rfind("y");
    string::size_type ipos = string::npos;
    if ( iposx == string::npos ) {
      ipos = iposy;
    } else {
      ipos = iposx;
      if ( iposy != string::npos && iposy > ipos ) {
        ipos = iposy;
      }
    }
    if ( namein.substr(0,5) == "hdraw" && ipos != string::npos ) {
      sslab << namein.substr(ipos);
    } else {
      sslab << hname;
    }
    dxprint(sslab.str());
    delete plab;
  }
  // Rebin and scale.
  if ( rebin > 1 ) {
    ph->Rebin(rebin);
    ph->Scale(1.0/rebin);
  }
  ph->SetStats(0);
  if ( xmax > xmin ) {
    ph->GetXaxis()->SetRangeUser(xmin, xmax);
  }
  if ( ymax > ymin ) {
    ph->SetMinimum(ymin);
    ph->SetMaximum(ymax);
  }
  // Draw the histo.
  TCanvas* pcan = new TCanvas;
  pcan->SetLeftMargin(0.11);
  pcan->SetRightMargin(0.03);
  ph->GetYaxis()->SetTitleOffset(1.3);
  ph->Draw();
  // Add a line at y=0.
  if ( ph->GetMaximum()*ph->GetMinimum() < 0.0 ) {
    TAxis* pax = ph->GetXaxis();
    double xlo = pax->GetBinLowEdge(pax->GetFirst());
    double xhi = pax->GetBinLowEdge(pax->GetLast()) + pax->GetBinWidth(pax->GetLast());
    TLine* pl = new TLine(xlo, 0.0, xhi, 0.0);
    pl->Draw();
  }
  return 0;
}

