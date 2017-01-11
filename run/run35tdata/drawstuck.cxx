// drawstuck.cxx

#include "DrawResult.h"
#include <iostream>
#include <sstream>
#include "draw.h"
#include "addaxis.h"
#include "mycolors.h"
#include "dxlabel.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TROOT.h"
#include "TFile.h"
#include "TLegend.h"
#include "TLine.h"

using std::cout;
using std::endl;
using std::ostringstream;

void drawstuck(DrawResult& res, int nchan =4, bool stuckonly =false,
               double limit =0.30, double limitsam =0.05) {
  double zmax = 0.50;
  static int iwin = 0;
  ++iwin;
  ostringstream sswinname;
  sswinname << "mean" << iwin;
  string swname = sswinname.str().c_str();
  TCanvas* pcan = new TCanvas(swname.c_str(), swname.c_str(), 1350, 500);
  pcan->GetPad(0)->SetRightMargin(0.03);
  pcan->GetPad(0)->SetLeftMargin(0.05);
  pcan->SetGridx();
  pcan->SetGridy();
  bool dostu = true;
  bool dost1 = true;
  bool dosam = true;
  bool dobad = true;
  bool doabs = true;
  if ( stuckonly ) dost1 = false;
  if ( stuckonly ) dosam = false;
  TH1* phstu = nullptr;
  TH1* phst1 = nullptr;
  TH1* phsam = nullptr;
  TH1* phbad = nullptr;
  TH1* phabs = nullptr;
  unsigned int nplot = 0;
  if ( dostu ) {
    phstu = res.stuck(nchan);
    phstu->SetStats(0);
    phstu->SetLineWidth(2);
    phstu->SetLineColor(myblue);
    phstu->GetYaxis()->SetTitleOffset(0.5);
    phstu->GetYaxis()->SetTickLength(0.013);
    phstu->SetMaximum(zmax);
    ++nplot;
  }
  if ( dost1 ) {
    phst1 = res.stuck(1);
    phst1->SetStats(0);
    phst1->SetLineWidth(1);
    phst1->SetLineColor(mybrown);
    phst1->SetLineStyle(2);
    ++nplot;
  }
  if ( dosam ) {
    phsam = res.same(nchan);
    phsam->SetStats(0);
    phsam->SetLineWidth(3);
    phsam->SetLineColor(myred);
    phsam->SetLineStyle(3);
    ++nplot;
  }
  if ( dobad ) {
    string hname = res.chanstat()->GetName();
    hname += "_bad";
    phbad = dynamic_cast<TH1*>(res.chanstat()->Clone(hname.c_str()));
    phbad->SetStats(0);
    phbad->SetLineColor(myyellow);
    phbad->SetFillColor(myyellow);
    phbad->SetLineColor(0);
    phbad->Scale(0.999*zmax);
    ++nplot;
  }
  if ( doabs ) {
    string hname = res.chanstat()->GetName();
    hname += "_absent";
    phabs = dynamic_cast<TH1*>(res.chanstat()->Clone(hname.c_str()));
    phabs->SetStats(0);
    phabs->SetLineColor(myltgray);
    phabs->SetFillColor(myltgray);
    phabs->SetLineColor(0);
    phabs->Scale(0.999*zmax);
    for ( int ich=1; ich<=phabs->GetNbinsX(); ++ich ) {
      double val = phabs->GetBinContent(ich)<0 ? 0.999*zmax : 0;
      phabs->SetBinContent(ich, val);
    }
    ++nplot;
  }
  TH1* phax = nullptr;
  if ( dostu ) phax = phstu;
  // Draw.
  phax->Draw();
  if ( dobad ) phbad->Draw("same");
  if ( doabs ) phabs->Draw("same");
  if ( dost1 ) phst1->Draw("same");
  if ( dostu ) phstu->Draw("same");
  if ( dosam ) phsam->Draw("same");
  phax->Draw("axis same");
  // Add legend.
  ostringstream sschan;
  sschan << nchan;
  string schan = sschan.str();
  double ylmax = 0.87;
  double yloff = 0.02;
  double ylfac = (0.22 - yloff)/4.0;
  double ylmin = ylmax - (yloff + nplot*ylfac);
  double xlmax = 0.18;
  if ( dosam ) xlmax = 0.20;
  TLegend* pleg = new TLegend(0.06, ylmin, 0.18, ylmax);
  string labst1 = "Sticky fraction";
  string labstu = "Sticky fraction " + schan;
  string labsam = "Repeat fraction " + schan;
  string labbad = "Bad";
  string lababs = "Absent";
  if ( dost1 ) pleg->AddEntry(phst1, labst1.c_str(), "l");
  if ( dostu ) pleg->AddEntry(phstu, labstu.c_str(), "l");
  if ( dosam ) pleg->AddEntry(phsam, labsam.c_str(), "l");
  if ( dobad ) pleg->AddEntry(phbad, labbad.c_str(), "f");
  if ( doabs ) pleg->AddEntry(phabs, lababs.c_str(), "f");
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->Draw();
  // Add extra axes.
  addaxis(phstu);
  // Add limit lines.
  if ( limit > 0.0 ) {
    double xmin = phax->GetXaxis()->GetXmin();
    double xmax = phax->GetXaxis()->GetXmax();
    TLine* pline = new TLine(xmin, limit, xmax, limit);
    pline->SetLineWidth(2);
    pline->SetLineStyle(2);
    pline->SetLineColor(myblue);
    pline->Draw();
  }
  if ( limitsam > 0.0 ) {
    double xmin = phax->GetXaxis()->GetXmin();
    double xmax = phax->GetXaxis()->GetXmax();
    TLine* pline = new TLine(xmin, limitsam, xmax, limitsam);
    pline->SetLineWidth(2);
    pline->SetLineStyle(2);
    pline->SetLineColor(myred);
    pline->Draw();
  }
  // Add label.
  dxlabel()->Draw();
  // Print.
  string ifname = res.filename;
  string::size_type ipos = ifname.find("_run");
  string::size_type jpos = ifname.find("_", ipos+1);
  string rname = ifname.substr(ipos+1, jpos-ipos-1);
  string fname = "stuck_" + rname + "_" + res.name + ".png";
  pcan->Print(fname.c_str());
}

DrawResult drawstuck(string name, int nchan =4, bool stuckonly =false,
                     double limit =0.30, double limitsam =0.05) {
  bool isBatch = gROOT->IsBatch();
  if ( ! isBatch ) gROOT->SetBatch(true);
  DrawResult res = draw(name, 1, 100, 10500, 11000);
  if ( ! isBatch ) gROOT->SetBatch(false);
  drawstuck(res, nchan, stuckonly, limit, limitsam);
  return res;
}
