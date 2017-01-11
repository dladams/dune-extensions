// drawmean.cxx

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
#include "TLine.h"
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

TCanvas* pccc = nullptr;

// if nst, use not-sticky-code truncated instead of all-code truncated
void drawmean(DrawResult& res, bool nst =true, bool tronly =true) {
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
  TH1* phmea = nullptr;
  TH1* phmnt = nullptr;
  TH1* phmen = nullptr;
  TH1* phbad = nullptr;
  TH1* phbdm = nullptr;
  TH1* phabs = nullptr;
  TH1* phabm = nullptr;
  bool domnt = true;
  bool domea = !tronly;
  bool domen = !tronly;
  bool dobad = true;
  bool doabs = true;
  unsigned int nplot = 0;
  double zmax = 50.0;
  double zmin = -zmax;
  if ( domea ) {
    phmea = res.mean();
    phmea->SetStats(0);
    phmea->SetLineWidth(2);
    phmea->SetLineStyle(3);
    phmea->SetLineColor(mybrown);
    phmea->SetMinimum(zmin);
    phmea->SetMaximum(zmax);
    ++nplot;
  }
  if ( domnt ) {
    phmnt = nst ? res.meanTruncatedNotSticky() : res.meanTruncated();
    phmnt->SetStats(0);
    phmnt->SetLineWidth(2);
    phmnt->SetLineColor(myblue);
    phmnt->SetLineStyle(1);
    phmnt->SetMinimum(zmin);
    phmnt->SetMaximum(zmax);
    ++nplot;
  }
  if ( domen ) {
    phmen = res.meanNotSticky();
    phmen->SetStats(0);
    phmen->SetLineWidth(2);
    phmen->SetLineColor(myred);
    phmen->SetLineStyle(2);
    phmen->SetMinimum(zmin);
    phmen->SetMaximum(zmax);
    ++nplot;
  }
  if ( dobad ) {
    TH1* phsta = res.chanstat();
    string hname = phsta->GetName();
    hname += "_bad";
    phbad = dynamic_cast<TH1*>(res.chanstat()->Clone(hname.c_str()));
    phbad->SetStats(0);
    phbad->SetLineColor(myyellow);
    phbad->SetFillColor(myyellow);
    phbad->SetLineColor(0);
    hname += "m";
    phbdm = dynamic_cast<TH1*>(phbad->Clone(hname.c_str()));
    for ( int ich=0; ich<=phbad->GetNbinsX(); ++ich ) {
      double valmax = phsta->GetBinContent(ich)==1 ? 0.999*zmax : 0;
      double valmin = phsta->GetBinContent(ich)==1 ? 0.999*zmin : 0;
      phbad->SetBinContent(ich, valmax);
      phbdm->SetBinContent(ich, valmin);
    }
    ++nplot;
  }
  if ( doabs ) {
    TH1* phsta = res.chanstat();
    string hname = phsta->GetName();
    hname += "_absent";
    phabs = dynamic_cast<TH1*>(res.chanstat()->Clone(hname.c_str()));
    phabs->SetStats(0);
    phabs->SetLineColor(myltgray);
    phabs->SetFillColor(myltgray);
    phabs->SetLineColor(0);
    hname += "m";
    phabm = dynamic_cast<TH1*>(phabs->Clone(hname.c_str()));
    for ( int ich=0; ich<=phbad->GetNbinsX(); ++ich ) {
      double valmax = phsta->GetBinContent(ich)==-1 ? 0.999*zmax : 0;
      double valmin = phsta->GetBinContent(ich)==-1 ? 0.999*zmin : 0;
      phabs->SetBinContent(ich, valmax);
      phabm->SetBinContent(ich, valmin);
    }
    ++nplot;
  }
  TH1* phax = nullptr;
  if ( dobad ) phax = phbad;
  if ( doabs ) phax = phabs;
  if ( domen ) phax = phmen;
  if ( domnt ) phax = phmnt;
  if ( domea ) phax = phmea;
  phax->GetYaxis()->SetTitleOffset(0.5);
  phax->GetYaxis()->SetTickLength(0.013);
  phax->Draw();
  if ( dobad ) phbad->Draw("same");
  if ( dobad ) phbdm->Draw("same");
  if ( doabs ) phabs->Draw("same");
  if ( doabs ) phabm->Draw("same");
  if ( domnt ) phmnt->Draw("same");
  if ( domen ) phmen->Draw("same");
  if ( domea ) phmea->Draw("same");
  phax->Draw("axis same");
  string ifname = res.filename;
  string::size_type ipos = ifname.find("_run");
  string::size_type jpos = ifname.find("_", ipos+1);
  string rname = ifname.substr(ipos+1, jpos-ipos-1);
  string fname = "mean_" + rname + "_" + res.name + ".png";
  // Add legend.
  double ylmax = 0.87;
  double ylmin = ylmax - (0.04 + nplot*0.035);
  TLegend* pleg = new TLegend(0.06, ylmin, 0.20, ylmax);
  string labmea = "mean all";
  string labmen = "mean not sticky";
  ostringstream ssts;
  ssts << res.truncsigma;
  string labmnt = nst ? "mean ns trunc" : "mean trunc";
  labmnt += ssts.str();
  string labbad = "Bad";
  string lababs = "Absent";
  if ( domea ) pleg->AddEntry(phmea, labmea.c_str(), "l");
  if ( domen ) pleg->AddEntry(phmen, labmen.c_str(), "l");
  if ( domnt ) pleg->AddEntry(phmnt, labmnt.c_str(), "l");
  if ( dobad ) pleg->AddEntry(phbad, labbad.c_str(), "f");
  if ( doabs ) pleg->AddEntry(phabs, lababs.c_str(), "f");
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->Draw();
  // Add axes.
  addaxis(phmea);
  double xmin = phax->GetXaxis()->GetXmin();
  double xmax = phax->GetXaxis()->GetXmax();
  TLine* pline = new TLine(xmin, 0.0, xmax, 0.0);
  pline->Draw();
  // Add label.
  dxlabel()->Draw();
  pcan->Print(fname.c_str());
}

DrawResult drawmean(string name, bool nst =true, bool tronly =true) {
  bool isBatch = gROOT->IsBatch();
  if ( ! isBatch ) gROOT->SetBatch(true);
  DrawResult res = draw(name, 1, 100);
  if ( ! isBatch ) gROOT->SetBatch(false);
  drawmean(res, nst, tronly);
  return res;
}
