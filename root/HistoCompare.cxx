// HistoCompare.cxx

#include "HistoCompare.h"
#include <iostream>
#include <sstream>

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::ostringstream;

namespace {

const vector<string> apas35t() {
  vector<string> apas = {"0u", "0v", "0z1", "0z2",
                         "1u", "1v", "1z1", "1z2",
                         "2u", "2v", "2z1", "2z2",
                         "3u", "3v", "3z1", "3z2"};
  return apas;
};

}

//**********************************************************************

namespace {

TH1* getHist(TDirectory* apdir, string name, int dbg) {
  const string myname = "getHist: ";
  TObject* pobj = 0;
  TDirectory* pdir = apdir;
  pdir->cd();
  pdir->Cd("DXDisplay");
  pdir = gDirectory;
  if ( dbg ) cout << "Looking for object." << endl;
  pdir->GetObject(name.c_str(), pobj);
  if ( pobj == 0 ) {
    size_t i1 = name.find('h') + 1;
    size_t i2 = name.find('_');
    if ( i1 ==1 && i2 != string::npos && i2 > i1) {
      string sevt = "event" + name.substr(i1, i2-i1);
      if ( dbg ) cout << myname << "Trying event directory " << sevt << " for " << name << endl;
      string savedir = pdir->GetPath();
      if ( pdir->cd(sevt.c_str()) ) {
        gDirectory->GetObject(name.c_str(), pobj);
      }
      pdir->cd(savedir.c_str());
    } else {
      cout << myname << "Name " << name << " is not in expected format: " << name << endl;
      cout << myname << "(i1=" << i1 << ", i2=" << i2 << ")" << endl;
    }
  }
  return dynamic_cast<TH1*>(pobj);
}

}  // end unnamed namespace

//**********************************************************************

HistoCompare::HistoCompare(string afname1, string afname2)
: fname1(afname1),
  fname2(afname2),
  dbg(1) { }

//**********************************************************************

int HistoCompare::compare(string hname) {
  nbad = 0;
  nbin = 0;
  const string myname = "HistoCompare: ";
  TFile* pf1 = TFile::Open(fname1.c_str(), "READ");
  if ( pf1 == nullptr || !pf1->IsOpen() ) {
    cout << myname << "Unable to open file " << fname1 << endl;
    return nbad = -1;
  }
  TFile* pf2 = TFile::Open(fname2.c_str(), "READ");
  if ( pf2 == nullptr || !pf2->IsOpen() ) {
    cout << myname << "Unable to open file " << fname2 << endl;
    return nbad = -2;
  }
  TH1* ph1 = getHist(pf1, hname, dbg);
  if ( ph1 == nullptr ) {
    cout << myname << "Unable to find " << hname << " in file " << fname1 << endl;
    return nbad = -3;
  }
  TH1* ph2 = getHist(pf2, hname, dbg);
  if ( ph2 == nullptr ) {
    cout << myname << "Unable to find " << hname << " in file " << fname2 << endl;
    return nbad = -4;
  }
  int nx = ph1->GetNbinsX();
  int ny = ph1->GetNbinsY();
  if ( ph1->GetNbinsX() != nx || ph1->GetNbinsY() != ny ) {
    cout << myname << "Histograms have different binning." << endl;
    return nbad = -5;
  }
  nbin = 0;
  nbad = 0;
  for ( int iy=0; iy<=ny; ++iy ) {
    for ( int ix=0; ix<=nx; ++ix ) {
      float val1 = ph1->GetBinContent(ix, iy);
      float val2 = ph1->GetBinContent(ix, iy);
      if ( dbg > 1 ) cout << "  Bin " << ix << "-" << iy << ": " << val1 << " " << val2 << endl;
      bool same = val1 == val2;
      if ( ! same ) ++ nbad;
      ++nbin;
    }
  }
  if ( nbad ) {
    cout << "Histograms " << hname << " differ in " << nbad << "/" << nbin << " bins." << endl;
  } else {
    cout << "Histograms " << hname << " match in " << nbin << " bins." << endl;
  }
  return nbad;
}

//**********************************************************************

int HistoCompare::compare35t(string hpre, int evt1, int evt2) {
  nbad = 0;
  if ( evt1 > 0 ) {
    for ( int evt=evt1; evt<=evt2; ++evt ) {
      ostringstream sspre;
      sspre << "h" << evt << "_" << hpre;
      nbad += compare35t(sspre.str());
    }
    return nbad;
  }
  nbin = 0;
  for ( string sapa : apas35t() ) {
    string hname = hpre + "apa" + sapa;
    HistoCompare hc(fname1, fname2);
    hc.dbg = 0;
    int icstat = hc.compare(hname);
    if ( icstat < 0 ) {
      return nbad = icstat;
    }
    nbad += hc.nbad;
    nbin += hc.nbin;
  }
  return nbad;
}

//**********************************************************************

