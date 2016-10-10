#include "draw.h"
#include "DrawResult.h"
#include "dxlabel.h"
#include "TCanvas.h"
#include "TText.h"
#include "TH1.h"
#include "TAxis.h"
#include "TLine.h"
#include "TLegend.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using std::string;
using std::cin;
using std::cout;
using std::ostringstream;
using std::fixed;
using std::setprecision;

int fig(int chan1, int chan2, string fout) {
  double zmax = 4000;
  DrawResult res1 = draw("h1_adcall", 0, zmax);
  DrawResult res2 = draw("h2_adcall", 0, zmax);
  TCanvas* pcan = nullptr;
  if ( fout.size() ) {
    ostringstream sslab;
    sslab << "Channels " << chan1 << " to " << chan2;
    TText* ptxt = new TText(0.20, 0.50, sslab.str().c_str());
    ptxt->SetNDC();
    TText* ptxt2 = new TText(0.20, 0.43, "David Adams, BNL  Oct 7, 2016");
    ptxt2->SetNDC();
    TCanvas* pcan = new TCanvas();
    ptxt->Draw();
    ptxt2->Draw();
    string fname = fout + "(";
    pcan->Print(fname.c_str(), "Title:Title");
  }
  int nChan = 0;
  int nChanGood = 0;
  TH1* phstuck = new TH1F("hstuck", "Stuck-bit fraction; Fraction; # Channels", 100, 0, 1.00001);
  phstuck->SetStats(0);
  phstuck->SetLineWidth(2);
  for ( int chan=chan1; chan<=chan2; ++chan ) {
    ++nChan;
    if ( chan1 < 0 ) {
      cout << "Channel> ";
      cout.flush();
      chan = -1;
      cin >> chan;
      if ( chan < 0 ) return 0;
      if ( fout.size() == 0 ) pcan = new TCanvas;
    }
    if ( pcan == 0 ) pcan = new TCanvas;
    TH1* ph1 = res1.signalChannel(chan);
    TH1* ph2 = res2.signalChannel(chan);
    // Skip bad channels.
    if ( ph1->GetNbinsX() < 2 ) continue;
    // Set the maximum for the y-axis.
    double ymax = ph1->GetMaximum();
    double ymax2 = ph2->GetMaximum();
    if ( ymax2 > ymax ) ymax = ymax2;
    ymax *= 1.03;
    ph1->SetMaximum(ymax);
    // Suppress small counts at either end of spectrum if
    // this does not remove too much.
    int nbin = ph1->GetNbinsX();
    double adclim = ymax/500.0;
    int bin1 = 0;
    for ( bin1=1; bin1<=nbin; ++bin1 ) {
      if ( ph1->GetBinContent(bin1) >= adclim ) break;
    }
    int bin2 = nbin;
    for ( bin2=nbin; bin2>0; --bin2 ) {
      if ( ph1->GetBinContent(bin2) >= adclim ) break;
    }
    if ( bin2 > bin1 + 20 ) ph1->GetXaxis()->SetRange(bin1, bin2);
    else {
      bin1 = 1;
      bin2 = nbin;
    }
    // Set histo titles and line styles.
    ostringstream sstitl;
    sstitl << "Channel " << chan;
    ph1->SetTitle(sstitl.str().c_str());
    ph1->SetStats(0);
    ph1->GetXaxis()->SetTitle("ADC count");
    ph1->GetYaxis()->SetTitle("# ticks");
    ph2->SetLineColor(1);
    ph1->SetLineWidth(2);
    ph2->SetLineColor(2);
    ph2->SetLineStyle(2);
    pcan->SetLeftMargin(0.12);
    pcan->SetRightMargin(0.06);
    ph1->GetYaxis()->SetTitleOffset(1.4);
    ph1->Draw();
    int adcmin = ph1->GetXaxis()->GetXmin();
    int adcmax = ph1->GetXaxis()->GetXmax();
    int sumTickStuck = 0;
    int sumTick = 0;
    for ( int bin=1; bin<nbin; ++bin ) {
      int adc = ph1->GetXaxis()->GetBinCenter(bin);
      int nTick = ph1->GetBinContent(bin);
      sumTick += nTick;
      int adcn = adc + 1;
      if ( 64*(adc/64) == adc ||
           64*(adcn/64) == adcn ) {
        sumTickStuck += nTick;
        TLine* pline = new TLine(adc+0.5, 0.0, adc+0.5, ymax);
        pline->SetLineStyle(3);
        pline->Draw();
      }
    }
    double fstuck = double(sumTickStuck)/double(sumTick);
    phstuck->Fill(fstuck);
    ostringstream sspstuck;
    sspstuck << fixed << setprecision(1) << 100*fstuck;
    string spstuck = sspstuck.str();
    cout << "Channel " << chan << ": stuck/total: " << sumTickStuck << "/" << sumTick
         << "(" << spstuck << "%)" << endl;
    TLegend* pleg = new TLegend(0.79, 0.77, 0.91, 0.88);
    pleg->AddEntry(ph1, "Event 1", "l");
    pleg->AddEntry(ph2, "Event 2", "l");
    pleg->SetBorderSize(0);
    pleg->Draw();
    dxlabel()->Draw();
    ph1->Draw("same");
    ph2->Draw("same");
    pcan->Update();
    if ( fout.size() ) {
      ostringstream sstoclab;
      sstoclab << "Title:Channel " << chan;
      pcan->Print(fout.c_str(), sstoclab.str().c_str());
    }
    ++nChanGood;
  }
  int nChanBad = nChan - nChanGood;
  pcan = new TCanvas;
  phstuck->Draw();
  dxlabel()->Draw();
  if ( fout.size() ) {
    pcan->Print(fout.c_str(), "Title:Stuck bit fraction");
  }
  if ( fout.size() ) {
    ostringstream sslab;
    sslab << "Channels " << chan1 << " to " << chan2;
    TText* ptxt = new TText(0.20, 0.50, sslab.str().c_str());
    ostringstream sslab2;
    sslab2 << "Bad/total channels: \n" << nChanBad << "/" << nChan;
    TText* ptxt2 = new TText(0.20, 0.43, sslab2.str().c_str());
    ptxt2->SetNDC();
    TCanvas* pcan = new TCanvas();
    ptxt->Draw();
    ptxt2->Draw();
    string fname = fout + ")";
    pcan->Print(fname.c_str(), "Title:Conclusion");
  }
  return 1;
}
