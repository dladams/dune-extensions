// drawrms.cxx

#include "DrawResult.h"
#include "draw.h"
#include "addaxis.h"
#include "mycolors.h"
#include "dxlabel.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TROOT.h"
#include "TFile.h"
#include "TLegend.h"
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

// if nst, use not-sticky-code truncated instead of all-code truncated
void drawrms(DrawResult& res, bool nst =true, bool tronly =true) {
  static int iwin = 0;
  ++iwin;
  ostringstream sswinname;
  sswinname << "rms" << iwin;
  string swname = sswinname.str().c_str();
  TCanvas* pcan = new TCanvas(swname.c_str(), swname.c_str(), 1350, 500);
  pcan->GetPad(0)->SetRightMargin(0.03);
  pcan->GetPad(0)->SetLeftMargin(0.05);
  pcan->SetGridx();
  pcan->SetGridy();
  TH1* phrms = nullptr;
  TH1* phrmt = nullptr;
  TH1* phrmn = nullptr;
  TH1* phbad = nullptr;
  TH1* phabs = nullptr;
  bool dormt = true;
  bool dorms = !tronly;
  bool dormn = !tronly;
  bool dobad = true;
  bool doabs = true;
  unsigned int nplot = 0;
  double zmax = 50.0;
  if ( dorms ) {
    phrms = res.rms();
    phrms->SetStats(0);
    phrms->SetLineWidth(2);
    phrms->SetLineStyle(3);
    phrms->SetLineColor(mybrown);
    phrms->SetMaximum(zmax);
    ++nplot;
  }
  if ( dormt ) {
    phrmt = nst ? res.rmsTruncatedNotSticky() : res.rmsTruncated();
    phrmt->SetStats(0);
    phrmt->SetLineWidth(2);
    phrmt->SetLineColor(myblue);
    phrmt->SetLineStyle(1);
    ++nplot;
  }
  if ( dormn ) {
    phrmn = res.rmsNotSticky();
    phrmn->SetStats(0);
    phrmn->SetLineWidth(2);
    phrmn->SetLineColor(myred);
    phrmn->SetLineStyle(2);
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
  if ( dobad ) phax = phbad;
  if ( doabs ) phax = phabs;
  if ( dormn ) phax = phrmn;
  if ( dormt ) phax = phrmt;
  if ( dorms ) phax = phrms;
  phax->GetYaxis()->SetTitleOffset(0.5);
  phax->GetYaxis()->SetTickLength(0.013);
  phax->Draw();
  if ( dobad ) phbad->Draw("same");
  if ( doabs ) phabs->Draw("same");
  if ( dormt ) phrmt->Draw("same");
  if ( dormn ) phrmn->Draw("same");
  if ( dorms ) phrms->Draw("same");
  phax->Draw("axis same");
  string ifname = res.filename;
  string::size_type ipos = ifname.find("_run");
  string::size_type jpos = ifname.find("_", ipos+1);
  string rname = ifname.substr(ipos+1, jpos-ipos-1);
  string fname = "rms_" + rname + "_" + res.name + ".png";
  // Add legend.
  double ylmax = 0.87;
  double ylmin = ylmax - (0.04 + nplot*0.035);
  TLegend* pleg = new TLegend(0.06, ylmin, 0.20, ylmax);
  string labrms = "RMS all";
  string labrmn = "RMS not sticky";
  ostringstream ssts;
  ssts << res.truncsigma;
  string labrmt = nst ? "RMS ns trunc" : "RMS trunc";
  labrmt += ssts.str();
  string labbad = "Bad";
  string lababs = "Absent";
  if ( dorms ) pleg->AddEntry(phrms, labrms.c_str(), "l");
  if ( dormn ) pleg->AddEntry(phrmn, labrmn.c_str(), "l");
  if ( dormt ) pleg->AddEntry(phrmt, labrmt.c_str(), "l");
  if ( dobad ) pleg->AddEntry(phbad, labbad.c_str(), "f");
  if ( doabs ) pleg->AddEntry(phabs, lababs.c_str(), "f");
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->Draw();
  // Add axes.
  addaxis(phrms);
  // Add label.
  dxlabel()->Draw();
  pcan->Print(fname.c_str());
}

DrawResult drawrms(string name, bool nst =true, bool tronly =true) {
  bool isBatch = gROOT->IsBatch();
  if ( ! isBatch ) gROOT->SetBatch(true);
  DrawResult res = draw(name, 1, 100);
  if ( ! isBatch ) gROOT->SetBatch(false);
  drawrms(res, nst, tronly);
  return res;
}
