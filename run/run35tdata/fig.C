#include "draw.h"
#include "DrawResult.h"
#include "dxlabel.h"
#include "mycolors.h"
#include "howStuck.h"
#include "TStyle.h"
#include "TDatime.h"
#include "TCanvas.h"
#include "TText.h"
#include "TLatex.h"
#include "TH1.h"
#include "TAxis.h"
#include "TLine.h"
#include "TLegend.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>

using std::string;
using std::cin;
using std::cout;
using std::ostringstream;
using std::fixed;
using std::setprecision;
using std::setw;
using std::vector;
using std::map;

typedef vector<TH1*> HistVector;
typedef vector<double> ValueVector;
typedef map<int, HistVector>  EventHistMap;
typedef map<int, ValueVector> EventValueMap;

vector<DrawResult> figres;

int myprec(double val) {
  if ( val < 2.e-8 ) return 1;
  if ( val < 2.e-7 ) return 9;
  if ( val < 2.e-6 ) return 8;
  if ( val < 2.e-5 ) return 7;
  if ( val < 2.e-4 ) return 6;
  if ( val < 2.e-3 ) return 5;
  if ( val < 2.e-2 ) return 4;
  if ( val < 2.e-1 ) return 3;
  else return 3;
}

int fig(int chan1, int chan2, string fout, unsigned int evn1=1, unsigned int evn2=0) {
  const string myname = "fig: ";
  double zmax = 4000;
  vector<int> evns;
  {
    unsigned int evn = evn1;
    evns.push_back(evn);
    while ( ++evn <= evn2 ) evns.push_back(evn);
  }
  vector<string> sevns;
  vector<DrawResult>& ress = figres;
  ress.clear();
  TLegend* pleg = nullptr;
  // Draw title page.
  TCanvas* pcan = nullptr;
  unsigned int nchan = chan2 - chan1 + 1;
  if ( fout.size() ) {
    TDatime date;
    string stime = date.AsString();
    string sftime = stime.substr(4,6) + ", " + stime.substr(20,4) + stime.substr(10,6);
    ostringstream sslab;
    double xtxt = 0.20;
    double ytxt = 0.70;
    double dytxt = 0.06;
    sslab << "Channels " << chan1 << " to " << chan2;
    vector<string> slines;
    slines.push_back("Stuck bit distributions");
    slines.push_back(dxlabel()->GetTitle());
    slines.push_back(sslab.str());
    slines.push_back("");
    slines.push_back("");
    slines.push_back("");
    slines.push_back("David Adams");
    slines.push_back("BNL");
    slines.push_back(sftime);
    pcan = new TCanvas();
    for ( string sline : slines ) {
      TText* ptxt = new TText(xtxt, ytxt, sline.c_str());
      ptxt->SetNDC();
      ptxt->Draw();
      ytxt -= dytxt;
    }
    string fname = fout + "(";
    pcan->Print(fname.c_str(), "Title:Title");
    pcan->Update();
  }
  // Draw notes page.
  if ( fout.size() ) {
    vector<string> slines;
    slines.push_back("Notes");
    slines.push_back("");
    slines.push_back("R_{stuck}^{N} is the fraction of ticks that are stuck in");
    slines.push_back("a stuck-bit range or N or more.");
    double xtxt = 0.20;
    double ytxt = 0.70;
    double dytxt = 0.06;
    pcan = new TCanvas();
    for ( string sline : slines ) {
      TLatex* ptxt = new TLatex(xtxt, ytxt, sline.c_str());
      ptxt->SetNDC();
      ptxt->Draw();
      ytxt -= dytxt;
    }
    pcan->Print(fout.c_str(), "Title:Notes");
    pcan->Update();
  }
  // Fetch ADC data for each event.
  for ( int evn : evns ) {
    ostringstream ssevn;
    ssevn << evn;
    string sevn = ssevn.str();
    sevns.push_back(sevn);
    string hname = "h" + sevn + "_adcall";
    DrawResult res = draw(hname, 0, zmax);
    if ( res.status ) {
      cout << myname << "ERROR: Unable to find histogram " << hname << endl;
      return 2;
    }
    ress.push_back(std::move(res));
  }
  DrawResult& res1 = ress[0];
  unsigned int nevt = ress.size();
  // Assign colors and line styles for each event.
  vector<int> evtcols;
  vector<int> evtlines;
  for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
    int icol = ievt % myncol;
    int col = mycols[icol];
    evtcols.push_back(col);
    evtlines.push_back(ievt+1);
  }
  // Set style for 1D plots..
  double padLeftMargin = gStyle->GetPadLeftMargin();
  double padRightMargin = gStyle->GetPadRightMargin();
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.06);
  // Initialize channel counters.
  int nChan = 0;
  int nChanGood = 0;
  int nChanR5gtp05 = 0;
  int nChanR5gtp10 = 0;
  // Create multi-channel histograms.
  // ... Stuck-bit fraction for the first event
  TH1* phstuck = new TH1F("hstuck", "Stuck-bit fraction; Fraction; # Channels", 100, 0, 1.00001);
  // Channel distribution of R for each threshold and event.
  vector<int> rthreshs = {1, 2, 3, 4, 5, 10, 50};
  EventHistMap hsratedistmap;
  for ( int rthresh : rthreshs ) {
    for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
      int evn = evns[ievt];
      ostringstream sshname;
      ostringstream sshtitl;
      sshname << "hsrate" << rthresh << "dist_evt" << evn;
      sshtitl << "Stuck-bit rate integral above " << rthresh << " for event " << evn
              << "; R_{stuck}^{" << rthresh << "}; # Channels";
      TH1* ph = new TH1F(sshname.str().c_str(), sshtitl.str().c_str(), 100, 0, 1.00001);
      ph->SetStats(0);
      if ( ievt == 0 ) ph->SetLineWidth(2);
      ph->SetLineColor(evtcols[ievt]);
      ph->SetLineStyle(evtlines[ievt]);
      hsratedistmap[rthresh].push_back(ph);
    }
  }
  // R vs. channel for each threshold and event.
  EventHistMap hsratechanmap;
  for ( int rthresh : rthreshs ) {
    for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
      int evn = evns[ievt];
      ostringstream sshname;
      ostringstream sshtitl;
      sshname << "hsrate" << rthresh << "chan_evt" << evn;
      sshtitl << "Stuck-bit rate integral above " << rthresh << " for event " << evn
              << ";Channel;R_{stuck}^{" << rthresh << "}";
      TH1* ph = new TH1F(sshname.str().c_str(), sshtitl.str().c_str(), nchan, chan1, chan2+1);
      ph->SetStats(0);
      if ( ievt == 0 ) ph->SetLineWidth(2);
      ph->SetLineColor(evtcols[ievt]);
      ph->SetLineStyle(evtlines[ievt]);
      ph->GetYaxis()->SetRangeUser(0,1);
      hsratechanmap[rthresh].push_back(ph);
    }
  }
  // Loop over channels.
  pcan = nullptr;
  for ( int chan=chan1; chan<=chan2; ++chan ) {
    //cout << "Channel " << chan << endl;
    ++nChan;
    if ( chan1 < 0 ) {
      cout << "Channel> ";
      cout.flush();
      chan = -1;
      cin >> chan;
      if ( chan < 0 ) return 2;
      if ( fout.size() == 0 ) pcan = new TCanvas;
    }
    // Fetch the ADC and stuck range histograms.
    vector<TH1*> phsigs;
    vector<TH1*> phstuckRanges;
    for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
      DrawResult& res = ress[ievt];
      string slab = "Title:Event " + sevns[ievt];
      TH1* phstuckRange;
      phsigs.push_back(res.signalChannel(chan, &phstuckRange));
      phstuckRanges.push_back(phstuckRange);
      // drawCanvas->Print(fout.c_str(), slab.c_str());   // Exhausts the memeory.
    }
    // Skip empty channels.
    TH1* ph1 = phsigs[0];
    if ( ph1->GetNbinsX() < 2 ) {
      cout << " Skipping bad channel " << chan << endl;
      continue;
    }
    unsigned int ntick = ress[0].timeChannel(chan)->GetNbinsX();
    // Evaluate the stuck range rates for each threshold and event.
    TH1* phsr1 = phstuckRanges[0];
    unsigned int nsr = phstuckRanges[0]->GetNbinsX();
    EventValueMap sratemap;
    for ( int rthresh : rthreshs ) {
      for ( unsigned int ievt=0; ievt<evns.size(); ++ievt ) {
        sratemap[rthresh].push_back(phstuckRanges[ievt]->Integral(rthresh+1, nsr+1)/ntick);
      }
    }
    // Build the stuck rate lines to display on plot.
    vector<string> sratelines;
    {
      ostringstream sssrate1;
      ostringstream sssrate5;
      ostringstream sssrate10;
      ostringstream sssrate50;
      sssrate1  << "R_{stuck}^{1} = "  << fixed << setprecision(myprec(sratemap[1][0]))  << sratemap[1][0];
      sssrate5  << "R_{stuck}^{5} = "  << fixed << setprecision(myprec(sratemap[1][5]))  << sratemap[5][0];
      sssrate10 << "R_{stuck}^{10} = " << fixed << setprecision(myprec(sratemap[1][10])) << sratemap[10][0];
      sssrate50 << "R_{stuck}^{50} = " << fixed << setprecision(myprec(sratemap[1][50])) << sratemap[50][0];
      sratelines.push_back(sssrate1.str());
      sratelines.push_back(sssrate5.str());
      sratelines.push_back(sssrate10.str());
      sratelines.push_back(sssrate50.str());
    }
    // Create the canvas for this channel.
    if ( pcan == 0 ) pcan = new TCanvas;
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
    for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
      TH1* phsig = phsigs[ievt];
      phsig->SetLineColor(evtcols[ievt]);
      phsig->SetLineStyle(evtlines[ievt]);
    }
    // Build labels.
    // Draw canvas.
    //pcan->SetLeftMargin(0.12);
    //pcan->SetRightMargin(0.06);
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
    for ( EventHistMap::value_type ihstvec : hsratedistmap ) {
      int rthresh = ihstvec.first;
      const HistVector& hdists = ihstvec.second;
      for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
        TH1* phdist = hdists[ievt];
        phdist->Fill(sratemap[rthresh][ievt]);
      }
    }
    TH1* phsrate5chan = hsratechanmap[5][0];
    int chanbin = phsrate5chan->GetXaxis()->FindBin(chan+0.01);
    double srate5 = sratemap[5][0];
    for ( EventHistMap::value_type ihstvec : hsratechanmap ) {
      int rthresh = ihstvec.first;
      const HistVector& hchans = ihstvec.second;
      for ( unsigned int ievt=0; ievt<nevt; ++ievt ) {
        hchans[ievt]->SetBinContent(chanbin, sratemap[rthresh][ievt]);
      }
    }
    // Display the stuck fraction for the first event.
    ostringstream sspstuck;
    sspstuck << fixed << setprecision(3) << fstuck;
    string spstuck = sspstuck.str();
    cout << "Channel " << chan << ": stuck/total: " << sumTickStuck << "/" << sumTick
         << "(" << spstuck << "%)" << endl;
    double xtxt = 0.79;
    double ytxt = 0.65;
    double dytxt = 0.045;
    int font = 42;
    if ( 0 ) {
      ostringstream ssfline;
      ssfline << "f_{stuck} = " << fixed << setprecision(3) << fstuck;
      TLatex* ptxt = new TLatex(xtxt, ytxt, ssfline.str().c_str());
      ptxt->SetNDC();
      ptxt->SetTextSize(0.03);
      ptxt->SetTextFont(font);
      ptxt->Draw();
      ytxt -= dytxt;
    }
    for ( string srateline : sratelines ) {
      cout << "  " << srateline << endl;
      TLatex* ptxt = new TLatex(xtxt, ytxt, srateline.c_str());
      ptxt->SetNDC();
      ptxt->SetTextSize(0.03);
      ptxt->SetTextFont(font);
      ptxt->Draw();
      ytxt -= dytxt;
    }
    double legymax = 0.88;
    double legymin = legymax - 0.03 - 0.04*phsigs.size();
    if ( pleg == nullptr ) {
      pleg = new TLegend(0.79, legymin, 0.91, legymax);
      for ( unsigned int ihst=0; ihst<phsigs.size(); ++ihst ) {
        string slab = "Event " + sevns[ihst];
        pleg->AddEntry(phsigs[ihst], slab.c_str(), "l");
      }
      pleg->SetBorderSize(0);
      pleg->SetFillStyle(0);
    }
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
    if ( srate5 > 0.05 ) ++nChanR5gtp05;
    if ( srate5 > 0.10 ) ++nChanR5gtp10;
  }
  dxlabel()->Draw();
  int nChanMissing = nChan - nChanGood;
  {
    pcan = new TCanvas;
    phstuck->Draw();
    dxlabel()->Draw();
    if ( fout.size() ) {
      pcan->Print(fout.c_str(), "Title:Stuck bit fraction");
    }
  }
  dxlabel()->Draw();
  // Add an R channel distribution plot for each threshold.
  {
   for ( EventHistMap::value_type ihstvec : hsratedistmap ) {
      int rthresh = ihstvec.first;
      const HistVector& hdists = ihstvec.second;
      TH1* phdist1 = dynamic_cast<TH1*>(hdists[0]->Clone());
      string titl = phdist1->GetTitle();
      titl = titl.substr(0, titl.find(" for event "));
      phdist1->SetTitle(titl.c_str());
      pcan = new TCanvas;
      phdist1->Draw();
      for ( unsigned int ievt=1; ievt<nevt; ++ievt ) {
        TH1* phdist = hdists[ievt];
        phdist->Draw("same");
      }
      pleg->Draw();
      dxlabel()->Draw();
      ostringstream sstitl;
      sstitl << "Title:R " << rthresh;
      if ( fout.size() ) {
        pcan->Print(fout.c_str(), sstitl.str().c_str());
      }
    }
  }
  dxlabel()->Draw();
  // Add an R vs. channel plot for each threshold.
  {
   for ( EventHistMap::value_type ihstvec : hsratechanmap ) {
      int rthresh = ihstvec.first;
      const HistVector& hchans = ihstvec.second;
      TH1* phchan1 = dynamic_cast<TH1*>(hchans[0]->Clone());
      string titl = phchan1->GetTitle();
      titl = titl.substr(0, titl.find(" for event "));
      phchan1->SetTitle(titl.c_str());
      pcan = new TCanvas;
      phchan1->Draw();
      for ( unsigned int ievt=1; ievt<nevt; ++ievt ) {
        TH1* phchan = hchans[ievt];
        phchan->Draw("same");
      }
      pcan->SetGridy();
      pleg->Draw();
      dxlabel()->Draw();
      ostringstream sstitl;
      sstitl << "Title:R^" << rthresh << " vs. channel";
      if ( fout.size() ) {
        pcan->Print(fout.c_str(), sstitl.str().c_str());
      }
    }
  }
  dxlabel()->Draw();
  if ( fout.size() ) {
    vector<string> slines;
    {
      ostringstream sslab;
      sslab << "Summary for channels " << chan1 << " to " << chan2;
      slines.push_back(sslab.str());
    }
    {
      ostringstream sslab;
      double rat = double(nChanMissing)/nChan;
      sslab << "   Missing/total channels: " << setw(4) << nChanMissing << "/" << nChan
            << " (" << fixed << setprecision(myprec(rat)) << rat << ")";
      slines.push_back(sslab.str());
    }
    {
      ostringstream sslab;
      double rat = double(nChanR5gtp10)/nChan;
      sslab << "(R_{stuck}^{5} > 0.10)/total channels: " << setw(4) << nChanR5gtp10 << "/" << nChan
            << " (" << fixed << setprecision(myprec(rat)) << rat << ")";
      slines.push_back(sslab.str());
    }
    {
      ostringstream sslab;
      double rat = double(nChanR5gtp05)/nChan;
      sslab << "(R_{stuck}^{5} > 0.05)/total channels: " << setw(4) << nChanR5gtp05 << "/" << nChan
            << " (" << fixed << setprecision(myprec(rat)) << rat << ")";
      slines.push_back(sslab.str());
    }
    double xtxt = 0.15;
    double ytxt = 0.70;
    double dytxt = 0.08;
    pcan = new TCanvas();
    for ( string sline : slines ) {
      cout << sline << endl;
      TLatex* ptxt = new TLatex(xtxt, ytxt, sline.c_str());
      ptxt->SetNDC();
      //ptxt->SetTextFont(43);
      ptxt->Print();
      ptxt->Draw();
      ytxt -= dytxt;
    }
    string fname = fout + ")";
    pcan->Print(fname.c_str(), "Title:Summary");
  }
  dxlabel()->Draw();
  gStyle->SetPadLeftMargin(padLeftMargin);
  gStyle->SetPadRightMargin(padRightMargin);
  return 0;
}
