// fig_mcnoise.cxx

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "dxopen.h"
#include "draw.h"
#include "DrawResult.h"
#include "mycolors.h"
#include "dxlabel.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TLegend.h"

using std::cout;
using std::endl;
using std::ostringstream;
using std::vector;

int fig_mcnoise(string sapa ="apa0z2", string sevtdat ="10", int useold =1) {
  const string myname = "fig_mcnoise: ";
  cout << "\n" << myname << "Processing " << sapa << endl;
  vector<string> sapas;
  if ( sapa == "apau" ) {
    sapas.push_back("apa0u");
    sapas.push_back("apa1u");
    sapas.push_back("apa2u");
    sapas.push_back("apa3u");
  }
  if ( sapa == "apav" ) {
    sapas.push_back("apa0v");
    sapas.push_back("apa1v");
    sapas.push_back("apa2v");
    sapas.push_back("apa3v");
  }
  if ( sapa == "apaz1" || sapa == "apaz" ) {
    sapas.push_back("apa0z1");
    sapas.push_back("apa1z1");
    sapas.push_back("apa2z1");
    sapas.push_back("apa3z1");
  }
  if ( sapa == "apaz2" || sapa == "apaz" ) {
    sapas.push_back("apa0z2");
    sapas.push_back("apa1z2");
    sapas.push_back("apa2z2");
    sapas.push_back("apa3z2");
  }
  if ( sapas.size() ) {
    for ( string newsapa : sapas ) {
      int rstat = fig_mcnoise(newsapa, sevtdat, useold);
      if ( rstat ) return rstat;
    }
    return 0;
  }
  unsigned int tick1 = 7000;
  unsigned int tick2 = 15000;
  string sticks = to_string(tick1) + "-" + to_string(tick2);
  string stickslab = "[" + to_string(tick1) + ", " + to_string(tick2) + "]";
  double zmax = 100.0;
  const unsigned int nres = 2;
  string jnames[nres];
  string hnames[nres];
  string fnames[nres];
  DrawResult ress[nres];
  string& jname1 = jnames[0];
  hnames[0] = "h" + sevtdat + "_dco" + sapa;
  hnames[1] = "h1_dco" + sapa;
  string srun = "13843";
  jnames[0] = "dxdwire-data_recowire-35tdata-nodco_run" + srun;
  string mclab;
  string smc;
  if ( useold ) {
    jnames[1] = "dxdwire-sim35t-long_recowire-35tsim-nodco_detsim-35t-long-nozs_g4-35tsim_genmu-35t";
    mclab = "Production MC tune";
    smc = "old";
  } else {
    mclab = "New MC tune";
    jnames[1] = "dxdwire-sim35t-long_recowire-35tsim-nodco_detsim-35t-long-noisy-nozs_g4-35tsim_genmu-35t";
    smc = "new";
  }
  unsigned int chan1 = 70;
  unsigned int chan2 = 84;
  chan2 = 70;
  bool allchan = false;
  TH2* fhists[2] = {nullptr, nullptr};
  for ( unsigned int ires=0; ires<nres; ++ires ) {
    string jname = jnames[ires];
    string hname = hnames[ires];
    string fname = "jobs/" + jname + "/" + jname + ".root";
    if ( dxopen(fname) ) {
      cout << myname << "Unable to open " << fname << endl;
      return 1;
    } else {
      cout << myname << "Opened file for " << jname << endl;
    }
    ress[ires] = draw(hname, 1, zmax, tick1, tick2-1);
    if ( ress[ires].hdraw == nullptr ) {
      cout << myname << "ERROR: Unable to find histogram " << hname << " for " << jname << endl;
      return 2;
    }
    fhists[ires] = ress[ires].freq();
    if ( chan2 <= chan1 ) {
      TH2* ph = ress[ires].hdraw;
      chan1 = ph->GetYaxis()->GetXmin() + 0.1;
      chan2 = ph->GetYaxis()->GetXmax() + 0.1;
      allchan = true;
    }
  }
  DrawResult& resdat = ress[0];
  DrawResult& resmc  = ress[1];
  ostringstream sschans;
  sschans << chan1 << "-" << chan2-1;
  string schans = sschans.str();
  string sopt = "same";
  // Fetch the data histograms.
  TH1* ph1 = nullptr;
  vector<TH1*> hdats;
  vector<TH1*> hmcs;
  double ymax = 0.0;
  double ymin = 1.e6;
  int rebin = 0;
  cout << myname << "Looping over data channels." << endl;
  TH1* phmc = nullptr;
  for ( unsigned int chan=chan1; chan<chan2; ++chan ) {
    TH1* ph = resdat.powerChannel(chan);
    if ( ph->GetEntries() == 0 ) {
      cout << "  Skipping empty histogram for channel " << chan << endl;
      continue;
    }
    cout << "  Channel " << chan << endl;
    if ( ph1 == nullptr ) {
      ph1 = ph;
      unsigned int nfbin = 50;
      rebin = float(ph1->GetNbinsX())/nfbin;
    }
    ph->Rebin(rebin);
    ph->SetLineColor(myblue);
    ph->SetLineWidth(2);
    ph->SetLineStyle(3);
    if ( ph->GetMinimum() < ymin ) ymin = ph->GetMinimum();
    if ( ph->GetMaximum() > ymax ) ymax = ph->GetMaximum();
    hdats.push_back(ph);
    // MC.
    if ( phmc == nullptr ) {
      phmc = resmc.powerChannel(chan);
      phmc->Rebin(rebin);
      if ( phmc->GetMinimum() < ymin ) ymin = phmc->GetMinimum();
      if ( phmc->GetMaximum() > ymax ) ymax = phmc->GetMaximum();
    }
  }
  if ( ph1 == nullptr ) {
    cout << myname << "Unable to find any data histograms with entries." << endl;
    return 3;
  }
  ymin *= 0.8;
  ymax *= 1.2;
  // Configure first histogram
  string stitle = "FFT power for " + sapa;
  ph1->SetTitle(stitle.c_str());
  ph1->SetMinimum(ymin);
  ph1->SetMaximum(ymax);
  ostringstream sslaby;
  double wbin = ph1->GetXaxis()->GetBinWidth(1);
  sslaby << "(ADC counts)^{2}/(" << int(wbin+0.5) << " kHz)";
  string slaby = sslaby.str();
  ph1->GetYaxis()->SetTitle(slaby.c_str());
  // Draw data histograms.
  TCanvas* pcan = new TCanvas;
  pcan->SetLeftMargin(0.13);
  pcan->SetRightMargin(0.05);
  pcan->SetLogy();
  ph1->DrawCopy();
  for ( TH1* ph : hdats ) ph->Draw("same");
  // Draw the MC
  phmc->SetLineColor(myred);
  phmc->SetLineWidth(2);
  phmc->Draw("same");
  // Add legend.
  TLegend* pleg = new TLegend(0.35, 0.78, 0.85, 0.87);
  string datlab = "Run 13843 event " + sevtdat + " " + stickslab;
  if ( ! allchan ) datlab += " " + schans;
  pleg->AddEntry(ph1, datlab.c_str(), "l");
  pleg->AddEntry(phmc, mclab.c_str(), "l");
  pleg->SetBorderSize(0);
  pleg->SetFillStyle(0);
  pleg->Draw();
  //dxlabel(mclab);
  // Print plot.
  string fname = "mcnoise_run" + srun + "_event" + sevtdat + "_ticks" + sticks + "_" + sapa + "_mc" + smc;
  fname += ".png";
  pcan->Print(fname.c_str());
  return 0;
}
    


