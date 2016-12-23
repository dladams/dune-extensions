// drawstuck.cxx

#include "DrawResult.h"
#include <sstream>
#include "draw.h"
#include "addaxis.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TROOT.h"
#include "TFile.h"
#include "TLegend.h"

using std::ostringstream;

void drawstuck(DrawResult& res, int nchan =4) {
  double zmax = 0.50;
  TCanvas* pcan = new TCanvas;
  pcan->SetWindowSize(1350, 500);
  pcan->GetPad(0)->SetRightMargin(0.03);
  pcan->GetPad(0)->SetLeftMargin(0.05);
  pcan->SetGridx();
  pcan->SetGridy();
  TH1* phstu = res.stuck(nchan);
  phstu->SetStats(0);
  phstu->GetYaxis()->SetTitleOffset(0.5);
  phstu->GetYaxis()->SetTickLength(0.013);
  phstu->SetMaximum(zmax);
  phstu->SetLineWidth(2);
  TH1* phst1 = res.stuck(1);
  phst1->SetStats(0);
  phst1->SetLineWidth(2);
  phst1->SetLineColor(1);
  phst1->SetLineStyle(2);
  TH1* phsam = res.same(nchan);
  phsam->SetStats(0);
  phsam->SetLineWidth(1);
  phsam->SetLineColor(2);
  TH1* phbad = dynamic_cast<TH1*>(res.chanstat()->Clone());
  phbad->SetStats(0);
  phbad->SetFillColor(5);
  phbad->SetLineColor(0);
  phbad->Scale(0.998*zmax);
  // Draw.
  phstu->Draw();
  phbad->Draw("same");
  phst1->Draw("same");
  phstu->Draw("same");
  phstu->Draw("same");
  phsam->Draw("same");
  phstu->Draw("axis same");
  // Add legend.
  ostringstream sschan;
  sschan << nchan;
  string schan = sschan.str();
  TLegend* pleg = new TLegend(0.06, 0.69, 0.20, 0.87);
  string labst1 = "Sticky fraction";
  string labstu = "Sticky fraction " + schan;
  string labsam = "Repeat fraction " + schan;
  string labbad = "Bad";
  pleg->AddEntry(phst1, labst1.c_str(), "l");
  pleg->AddEntry(phstu, labstu.c_str(), "l");
  pleg->AddEntry(phsam, labsam.c_str(), "l");
  pleg->AddEntry(phbad, labbad.c_str(), "f");
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->Draw();
  // Add extra axes.
  addaxis(phstu);
  // Print.
  string ifname = res.filename;
  string::size_type ipos = ifname.find("_run");
  string::size_type jpos = ifname.find("_", ipos+1);
  string rname = ifname.substr(ipos+1, jpos-ipos-1);
  string fname = "stuck_" + rname + "_" + res.name + ".png";
  pcan->Print(fname.c_str());
}

DrawResult drawstuck(string name, int nchan =4) {
  bool isBatch = gROOT->IsBatch();
  if ( ! isBatch ) gROOT->SetBatch(true);
  DrawResult res = draw(name, 1, 100, 10500, 11000);
  if ( ! isBatch ) gROOT->SetBatch(false);
  drawstuck(res, nchan);
  return res;
}
