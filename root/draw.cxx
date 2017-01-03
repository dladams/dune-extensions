// draw.cxx
//
// David Adams
// October 2015
//
// Root function to draw the channel-tick histograms produced
// by the dune_extensions package.

#include "draw.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "TDirectory.h"
#include "TROOT.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TPaletteAxis.h"
#include "palette.h"
#include "addaxis.h"
#include "fix2dcanvas.h"
#include "dxhist.h"
#include "dxlabel.h"
#include "dxprint.h"
#include "dxopen.h"

using std::string;
using std::ostringstream;
using std::cout;
using std::endl;
using std::hex;
using std::dec;

TCanvas* drawCanvas = nullptr;

DrawResult draw(std::string name, int how, double zmax,
                double xmin, double xmax, double ymin, double ymax) {
  DrawResult res(xmin, xmax);
  // Record the histogram name.
  dxhist(name);
  // Build label.
  string plname;
  TLatex* plab = dxlabel();
  if ( plab != 0 ) {
    ostringstream sslab;
    string sep = "_";
    sslab << plab->GetTitle() << sep << name;
    if ( xmax > xmin ) sslab << sep << xmin << sep << xmax;
    if ( ymax > ymin ) sslab << sep << ymin << sep << ymax;
    if ( zmax > 0.0 ) sslab << sep << zmax;
    plname += "_";
    dxprint(sslab.str());
    delete plab;
  }
  const string myname = "draw: ";
  if ( name == "help" ) {
    cout << myname << endl;
    cout << myname << "Usage: draw(hname, how, zmax, xmin, xmax, ymin, ymax);" << endl;
    cout << myname << "how = 0 - new standard canvas" << endl;
    cout << myname << "      1 - new stretched canvas" << endl;
    cout << myname << "     -1 - add to existing histogram/canvas" << endl;
    cout << myname << "      * - draw on current canvas" << endl;
    return res;
  }
  double xh1 = 0.05;
  double xh2 = 0.94;
  TObject* pobj = nullptr;
  if ( gDXFile != nullptr ) {
    gDXFile->cd("DXDisplay");
  } else {
    cout << myname << "WARNING: gDXFile is null." << endl;
  }
  gDirectory->GetObject(name.c_str(), pobj);
  TH1* phped = nullptr;
  TH1* phbad = nullptr;
  if ( pobj == 0 ) {
    size_t i1 = name.find('h') + 1;
    size_t i2 = name.find('_');
    if ( i1 ==1 && i2 != string::npos && i2 > i1) {
      string sevt = "event" + name.substr(i1, i2-i1);
      cout << myname << "Trying event directory " << sevt << " for " << name << endl;
      string savedir = gDirectory->GetPath();
      if ( gDirectory->cd(sevt.c_str()) ) {
        gDirectory->GetObject(name.c_str(), pobj);
        string pedname = name + "_ped";
        TObject* pobjped = nullptr;
        gDirectory->GetObject(pedname.c_str(), pobjped);
        phped = dynamic_cast<TH1*>(pobjped);
        string badname = name + "_badchan";
        TObject* pobjbad = nullptr;
        gDirectory->GetObject(badname.c_str(), pobjbad);
        phbad = dynamic_cast<TH1*>(pobjbad);
      } else {
        cout << myname << "Directory not found in " << gDirectory->GetPath() << endl;
      }
      gDirectory->cd(savedir.c_str());
    } else {
      cout << myname << "Name " << name << " is not in expected format: " << name << endl;
      cout << myname << "(i1=" << i1 << ", i2=" << i2 << ")" << endl;
    }
  }
  if ( pobj == 0 ) {
    cout << myname << "Object not found: " << name << endl;
    res.status = 1;
    return res;
  }
  // Record the pedestals.
  if ( phped != nullptr ) {
    for ( int ibin=1; ibin<=phped->GetNbinsX(); ++ibin ) {
      res.pedestals.push_back(phped->GetBinContent(ibin));
    }
  }
  // Record the channel status histogram and the status for each channel.
  res.hchanstat = phbad;
  if ( phbad != nullptr ) {
    for ( int ibin=1; ibin<=phbad->GetNbinsX(); ++ibin ) {
      res.chanstats.push_back(phbad->GetBinContent(ibin));
    }
  }
  TCanvas*& pcan = drawCanvas;
  static TH2* phdraw;
  TH2* phnew = dynamic_cast<TH2*>(pobj);
  if ( phnew == 0 ) {
    cout << myname << "Object is not TH2: " << name << endl;
    res.status = 2;
    return res;
  }
  if ( phnew->GetEntries() == 0 ) {
    cout << myname << "Histogram is empty: " << name << endl;
    res.status = 3;
    return res;
  }
  // Early draw to make palette available
  double zmin = phnew->GetMinimum();
  bool isflag = name.find("flag") != string::npos;
  if ( zmin < 0.0 ) {
    if ( (name.find("raw") != string::npos)  && zmax > 0 && zmax <=20 ) palette(3);
    else palette(2);
  }
  else if ( isflag ) palette(31);
  else palette(1);
  // Draw canvas and determine set palette parameters.
  double palx1 = 0.865;
  double palx2 = 0.91;
  double paltoff = 1.00;
  double axyoff = 1.0;
  double toff = 1.00;
  string dopt = "colz";
  bool add = false;
  double tsize = 0.0;
  static int ccount = 0;
  if ( how == 0 ) {
    // Standard canvas.
    pcan = new TCanvas;
    fix2dcanvas();
  } else if ( how == 1 || how == 2 ) {
    ++ccount;
    ostringstream sscname;
    sscname << "mycan" << ccount;
    string cname = sscname.str();
    // Extra-wide canvas.
    if ( how == 1 ) {
      pcan = new TCanvas(cname.c_str(), cname.c_str(), 1600, 500);
      pcan->SetLeftMargin(0.05);
      pcan->SetLeftMargin(0.05);
      axyoff = 0.70;
    } else {
      pcan = new TCanvas(cname.c_str(), cname.c_str(), 1600, 800);
      tsize = 0.025;
      pcan->SetLeftMargin(0.07);
      axyoff = 1.20;
      xh2 = 0.90;
    }
    fix2dcanvas();
    pcan->SetRightMargin(1.0-xh2);
    palx1 = xh2;
    palx2 = xh2 + 0.015;
    paltoff = 0.50;
  } else if ( how == -1 ) {
    if ( pcan == 0 || phdraw == 0 ) {
      cout << "There is no existing histogram!" << endl;
      res.status = 4;
      return res;
    }
    add = true;
  } else {
    if ( pcan == 0 ) {
      pcan = new TCanvas;
      fix2dcanvas();
    } else {
      cout << "Reusing last canvas: " << pcan->GetName() << endl;
    }
  }
  static int hcount = 0;
  if ( add ) {
    cout << "Adding histogram " << phnew->GetName() << " to " << phdraw->GetName() << endl;
    // Loop over bins in the new histogram and add to corresponding bin in the
    // histogram to be displayed. Under and overflow bins are ignored.
    for ( int iy=1; iy<=phnew->GetNbinsY(); ++iy ) {
      double y = phnew->GetYaxis()->GetBinCenter(iy);
      for ( int ix=1; ix<=phnew->GetNbinsX(); ++ix ) {
        double x = phnew->GetXaxis()->GetBinCenter(ix);
        double val = phnew->GetBinContent(ix, iy);
        if ( val != 0.0 ) {
          //cout << "Adding " << x << ", " << y << ": " << val << endl;
          phdraw->Fill(x, y, val);
        }
      }
    }
  } else {
    ++hcount;
    ostringstream sshname;
    sshname << "hdraw" << hcount;
    string hname = sshname.str();
    const char* dname = hname.c_str();
    cout << "Creating histogram " << hname << endl;
    gROOT->cd();
    if ( !gDirectory->mkdir(dname) || !gDirectory->cd(dname) ) {
      cout << "Unable to create or move to directory " << dname << endl;
      cout << "Current directory is " << gDirectory->GetName() << endl;
      res.status = 5;
      return res;
    }
    cout << myname << "Created histogram directory: " << gDirectory->GetName() << endl;
    phdraw = dynamic_cast<TH2*>(phnew->Clone(hname.c_str()));
    //phdraw->SetDirectory(nullptr);
    // Set axis parameters.
    //ph2->SetTitleOffset(toff);   // Sets x-axis label!!
    phdraw->SetTickLength(0.010, "X");
    phdraw->SetTickLength(0.010, "Y");
    phdraw->GetYaxis()->SetTitleOffset(axyoff);
    if ( tsize > 0.0 ) {
      phdraw->GetXaxis()->SetLabelSize(tsize);
      phdraw->GetYaxis()->SetLabelSize(tsize);
      phdraw->GetZaxis()->SetLabelSize(tsize);
      phdraw->GetXaxis()->SetTitleSize(tsize);
      phdraw->GetYaxis()->SetTitleSize(tsize);
      phdraw->SetTitleSize(tsize);
    }
    // Add projection hists.
    string hprx_name = phdraw->GetName();
    string hpry_name = phdraw->GetName();
    hprx_name += "x";
    hpry_name += "y";
    string ylab = phdraw->GetZaxis()->GetTitle();
    string::size_type ipos = ylab.rfind("]");
    if ( ipos == string::npos ) ipos = ylab.size();
    string ylabprx = ylab;
    ylabprx.insert(ipos, "/tick");
    string ylabpry = ylab;
    ylabpry.insert(ipos, "/channel");
    // Make a tick-projection histogram for all channels.
    int xbin1 = 1;
    int xbin2 = phdraw->GetNbinsX();
    int ybin1 = 1;
    int ybin2 = phdraw->GetNbinsY();
    if ( xmin < xmax ) {
      xbin1 = xmin + 1;
      xbin2 = xmax;
    }
    if ( ymin < ymax ) {
      ybin1 = ymin + 1;
      ybin2 = ymax;
    }
    TH1* hprx = phdraw->ProjectionX(hprx_name.c_str());
    if ( xmax > xmin ) hprx->GetXaxis()->SetRangeUser(xmin, xmax);
    string ytitle;
    hprx->GetYaxis()->SetTitle(ylabprx.c_str());
    hprx->SetStats(0);
    res.hdrawx = hprx;
    // Make a tick-projection histogram for each channel.
    string hname1;
    string hname2;
    for ( int ibin=1; ibin<=phdraw->GetNbinsY(); ++ibin ) {
      int ich = ibin - 1;
      ostringstream ssname;
      ssname << hprx_name;
      if ( ich < 100 ) ssname << "0";
      if ( ich < 10 ) ssname << "0";
      ssname << ich;
      string hname = ssname.str().c_str();
      if ( hname1.size() == 0 ) hname1 = hname;
      hname2 = hname;
      hprx = phdraw->ProjectionX(ssname.str().c_str(), ibin, ibin);
      ostringstream sstitle;
      sstitle << phdraw->GetTitle() << " channel " << ich;
      hprx->SetTitle(sstitle.str().c_str());
      if ( xmax > xmin ) hprx->GetXaxis()->SetRangeUser(xmin, xmax);
      hprx->GetYaxis()->SetTitle(ylabprx.c_str());
      hprx->SetStats(0);
      res.hdrawxChan.push_back(hprx);
    }
    // Make a channel-projection histogram.
    TH1* hpry = phdraw->ProjectionY(hpry_name.c_str(), xbin1, xbin2);
    if ( ymax > ymin ) hpry->GetYaxis()->SetRangeUser(ymin, ymax);
    hpry->GetYaxis()->SetTitle(ylabpry.c_str());
    ostringstream sstpry;
    sstpry << hpry->GetTitle();
    if ( xmax > xmin ) sstpry << " ticks " << xmin << "-" << xmax-1;
    hpry->SetTitle(sstpry.str().c_str());
    hpry->SetStats(0);
    res.hdrawy = hpry;
    cout << "Channel projection histogram: " << hpry_name << endl;
    cout << "Time projection histogram: " << hprx_name << endl;
    cout << "Time projection histogram for each channel: " << hname1 << ", ..., " << hname2 << endl;
  }
  // Draw.
  //dla phdraw->Draw(dopt.c_str());
  //phdraw->Draw("axis");
  // Retrieve the palette axis.
  gPad->Update();
  //phdraw->GetListOfFunctions()->Print(); 
  if ( !add &&  xmax > xmin ) {
    phdraw->GetXaxis()->SetRangeUser(xmin, xmax);
  }
  if ( !add &&  ymax > ymin ) {
    phdraw->GetYaxis()->SetRangeUser(ymin, ymax);
  }
  if ( !add &&  zmax > 0.0 ) {
    cout << "Changing z limits to zmax = " << zmax << "." << endl;
    phdraw->SetMaximum(zmax);
    if ( phdraw->GetMinimum() < 0.0 ) {
      phdraw->SetMinimum(-zmax);
    }
    //dla phdraw->Draw(dopt.c_str());
  }
  phdraw->Draw(dopt.c_str());
  // Set palette parameters. First draw to make sure palette is present.
  gPad->Update();
  TPaletteAxis* ppalax = dynamic_cast<TPaletteAxis*>(phdraw->GetListOfFunctions()->FindObject("palette")); 
  if ( ppalax == 0 ) {
    cout << myname << "Unable to retrieve palette." << endl;
    res.status = 6;
    return res;
  }
  //cout << "Palette axis: " << hex << long(ppalax) << dec << endl;
  ppalax->SetX1NDC(palx1);
  ppalax->SetX2NDC(palx2);
  ppalax->SetTitleOffset(paltoff);
  // Add top and right axis.
  addaxis(phdraw);
  res.hdraw = phdraw;
  if ( name.find("_raw") ) res.havePedestal = true;
  res.name = name;
  res.filename = gDXFile->GetName();
  cout << myname << "Done." << endl;
  return res;
}
