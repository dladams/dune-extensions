#include "draw.h"
#include "DrawResult.h"
#include "dxlabel.h"
#include "mycolors.h"
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

bool howStuck(int adc) {
  if ( 64*(adc/64) == adc ) return 1;
  int adcn = adc + 1;
  if ( 64*(adcn/64) == adcn ) return 2;
  return 0;
}

int fig(int chan1, int chan2, string fout) {
  double zmax = 4000;
  vector<int> evns = {1, 2, 3};
  vector<string> sevns;
  vector<DrawResult> ress;
  for ( int evn : evns ) {
    ostringstream ssevn;
    ssevn << evn;
    string sevn = ssevn.str();
    sevns.push_back(sevn);
    string hname = "h" + sevn + "_adcall";
    DrawResult res = draw(hname, 0, zmax);
    ress.push_back(std::move(res));
  }
  DrawResult& res1 = ress[0];
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
    vector<TH1*> phsigs;
    vector<TH1*> phstuckRanges;
    for ( DrawResult& res : ress ) {
      TH1* phstuckRange;
      phsigs.push_back(res.signalChannel(chan), &phstuckRange);
      phstuckRanges.push_back(phstuckRange);
    }
    TH1* ph1 = phsigs[0];
    if ( pcan == 0 ) pcan = new TCanvas;
    // Skip bad channels.
    if ( ph1->GetNbinsX() < 2 ) continue;
    // Set the maximum for the y-axis.
    double ymax = 0.0;
    for ( TH1* phsig : phsigs ) {
      double ymaxnew = phsig->GetMaximum();
      if ( ymaxnew > ymax ) ymax = ymaxnew;
    }
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
    ph1->SetLineWidth(2);
    for ( unsigned int ihst=0; ihst<phsigs.size(); ++ihst ) {
      TH1* phsig = phsigs[ihst];
      int icol = ihst % myncol;
      int col = mycols[icol];
      phsig->SetLineColor(col);
      phsig->SetLineStyle(ihst+1);
    }
    pcan->SetLeftMargin(0.12);
    pcan->SetRightMargin(0.06);
    ph1->GetYaxis()->SetTitleOffset(1.4);
    ph1->Draw();
    int adcmin = ph1->GetXaxis()->GetXmin();
    int adcmax = ph1->GetXaxis()->GetXmax();
    int sumTickStuck = 0;
    int sumTick = 0;
    for ( int bin=1; bin<=nbin; ++bin ) {
      int adc = ph1->GetXaxis()->GetBinCenter(bin);
      int nTick = ph1->GetBinContent(bin);
      sumTick += nTick;
      if ( howStuck(adc) ) {
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
    double legymax = 0.88;
    double legymin = legymax - 0.03 - 0.04*phsigs.size();
    TLegend* pleg = new TLegend(0.79, legymin, 0.91, legymax);
    for ( unsigned int ihst=0; ihst<phsigs.size(); ++ihst ) {
      string slab = "Event " + sevns[ihst];
      pleg->AddEntry(phsigs[ihst], slab.c_str(), "l");
    }
    pleg->SetBorderSize(0);
    pleg->Draw();
    dxlabel()->Draw();
    for ( TH1* phsig : phsigs ) {
      phsig->Draw("same");
    }
    pcan->Update();
    if ( fout.size() ) {
      ostringstream sstoclab;
      sstoclab << "Title:Channel " << chan;
      pcan->Print(fout.c_str(), sstoclab.str().c_str());
    }
    ++nChanGood;
  }
  dxlabel()->Draw();
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
  dxlabel()->Draw();
  return 1;
}
