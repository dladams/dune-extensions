// drawrms.cxx

#include "DrawResult.h"
#include "draw.h"
#include "addaxis.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TROOT.h"
#include "TFile.h"
#include "TLegend.h"

#include <iostream>
using std::cout;
using std::endl;

void drawrms(DrawResult& res) {
  TCanvas* pcan = new TCanvas;
  pcan->SetWindowSize(1350, 500);
  pcan->GetPad(0)->SetRightMargin(0.03);
  pcan->GetPad(0)->SetLeftMargin(0.05);
  pcan->SetGridx();
  pcan->SetGridy();
  TH1* phrms = res.rms();
  phrms->SetStats(0);
  phrms->GetYaxis()->SetTitleOffset(0.5);
  phrms->GetYaxis()->SetTickLength(0.013);
  phrms->SetMaximum(50);
  phrms->SetLineWidth(2);
  TH1* phrmt = res.rmsTruncated();
  phrmt->SetStats(0);
  phrmt->SetLineWidth(2);
  phrmt->SetLineColor(3);
  phrmt->SetLineStyle(3);
  TH1* phrmn = res.rmsNotSticky();
  phrmn->SetStats(0);
  phrmn->SetLineWidth(2);
  phrmn->SetLineColor(2);
  phrmn->SetLineStyle(2);
  TH1* phbad = dynamic_cast<TH1*>(res.chanstat()->Clone());
  phbad->SetStats(0);
  phbad->SetFillColor(5);
  phbad->SetLineColor(0);
  phbad->Scale(49.9);
  phrms->Draw();
  phbad->Draw("same");
  phrms->Draw("same");
  phrmn->Draw("same");
  phrmt->Draw("same");
  phrms->Draw("axis same");
  string ifname = res.filename;
  string::size_type ipos = ifname.find("_run");
  string::size_type jpos = ifname.find("_", ipos+1);
  string rname = ifname.substr(ipos+1, jpos-ipos-1);
  string fname = "rms_" + rname + "_" + res.name + ".png";
  // Add legend.
  TLegend* pleg = new TLegend(0.06, 0.69, 0.20, 0.87);
  string labrms = "RMS all";
  string labrmn = "RMS not sticky";
  string labrmt = "RMS truncated";
  string labbad = "Bad";
  pleg->AddEntry(phrms, labrms.c_str(), "l");
  pleg->AddEntry(phrmn, labrmn.c_str(), "l");
  pleg->AddEntry(phrmt, labrmt.c_str(), "l");
  pleg->AddEntry(phbad, labbad.c_str(), "f");
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->Draw();
  // Add axes.
  addaxis(phrms);
  pcan->Print(fname.c_str());
}

DrawResult drawrms(string name) {
  bool isBatch = gROOT->IsBatch();
  if ( ! isBatch ) gROOT->SetBatch(true);
  DrawResult res = draw(name, 1, 100, 10500, 11000);
  if ( ! isBatch ) gROOT->SetBatch(false);
  drawrms(res);
  return res;
}
