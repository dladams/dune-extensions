// HistoCompare.cxx

#include "HistoCompare.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::ostringstream;
using std::setw;

namespace {

const vector<string> apasnone() {
  vector<string> apas;
  return apas;
};

const vector<string> apas35t() {
  vector<string> apas = {"0u", "0v", "0z1", "0z2",
                         "1u", "1v", "1z1", "1z2",
                         "2u", "2v", "2z1", "2z2",
                         "3u", "3v", "3z1", "3z2"};
  return apas;
};

const vector<string> apasfd126() {
  vector<string> apas = { "0u",  "0v",  "0z1",  "0z2",
                          "1u",  "1v",  "1z1",  "1z2",
                          "2u",  "2v",  "2z1",  "2z2",
                          "3u",  "3v",  "3z1",  "3z2",
                          "4u",  "4v",  "4z1",  "4z2",
                          "5u",  "5v",  "5z1",  "5z2",
                          "6u",  "6v",  "6z1",  "6z2",
                          "7u",  "7v",  "7z1",  "7z2",
                          "8u",  "8v",  "8z1",  "8z2",
                          "9u",  "9v",  "9z1",  "9z2",
                         "10u", "10v", "10z1", "10z2",
                         "11u", "11v", "11z1", "11z2"};
  return apas;
};

const vector<string> apas(string sdet) {
  if ( sdet == "35t" ) return apas35t();
  if ( sdet == "fd126" ) return apasfd126();
  assert(false);
  return apasnone();
}

};  // end unnamed namespace

//**********************************************************************

namespace {

TH1* getHist(TDirectory* apdir, string name, int dbg) {
  const string myname = "getHist: ";
  TObject* pobj = 0;
  TDirectory* pdir = apdir;
  pdir->cd();
  pdir->Cd("DXDisplay");
  pdir = gDirectory;
  if ( dbg > 2 ) cout << "Looking for object." << endl;
  pdir->GetObject(name.c_str(), pobj);
  if ( pobj == 0 ) {
    size_t i1 = name.find('h') + 1;
    size_t i2 = name.find('_');
    if ( i1 ==1 && i2 != string::npos && i2 > i1) {
      string sevt = "event" + name.substr(i1, i2-i1);
      if ( dbg > 2 ) cout << myname << "Trying event directory " << sevt << " for " << name << endl;
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

HistoCompare::HistoCompare(string afname1, string afname2, int adbg, TFile* pfile)
: fname1(afname1),
  fname2(afname2),
  dbg(adbg),
  nbin(0), nbinbad(0), nhst(0), nhstbad(0),
  m_pfile(pfile), m_closefile(false), m_phdiff(0) {
  string myname = "HistoCompare::ctor: ";
  if ( m_pfile == 0 ) {
    m_pfile = TFile::Open("HistoCompare.root", "RECREATE");
    m_closefile = true;
  }
  if ( m_pfile == 0 || ! m_pfile->IsOpen() ) {
    cout << myname << "Unable to open output histogram file." << endl;
    return;
  }
  string title = fname1 + " - " + fname2;
  m_phdiff = dynamic_cast<TH1*>(m_pfile->Get("hdiff"));
  if ( m_phdiff != 0 ) {
    string oldtitle = m_phdiff->GetTitle();
    if ( oldtitle != title ) {
      cout << myname << "Old histogram is not consistent and will be removed." << endl;
      cout << myname << "Old: " << oldtitle << endl;
      cout << myname << "New: " << title << endl;
      m_pfile->Delete("hdiff");
      m_phdiff = 0;
    }
  }
  if ( m_phdiff == 0 ) {
    m_phdiff = new TH1F("hdiff", title.c_str(), 201, -100.5, 100.5);
  }
}

//**********************************************************************

HistoCompare::~HistoCompare() {
  if ( m_closefile && m_pfile!=0 ) m_pfile->Close();
}

//**********************************************************************

int HistoCompare::compare(string hname, double fdiffmax, double* pmaxfdiff) {
  nbin = 0;
  nhst = 0;
  nbinbad = 0;
  nhstbad = 0;
  const string myname = "HistoCompare: ";
  TFile* pf1 = TFile::Open(fname1.c_str(), "READ");
  if ( pf1 == nullptr || !pf1->IsOpen() ) {
    cout << myname << "Unable to open file " << fname1 << endl;
    return nhstbad = nbinbad = -1;
  }
  TFile* pf2 = TFile::Open(fname2.c_str(), "READ");
  if ( pf2 == nullptr || !pf2->IsOpen() ) {
    cout << myname << "Unable to open file " << fname2 << endl;
    return nhstbad = nbinbad = -2;
  }
  TH1* ph1 = getHist(pf1, hname, dbg);
  if ( ph1 == nullptr ) {
    cout << myname << "Unable to find " << hname << " in file " << fname1 << endl;
    return nhstbad = nbinbad = -3;
  }
  TH1* ph2 = getHist(pf2, hname, dbg);
  if ( ph2 == nullptr ) {
    cout << myname << "Unable to find " << hname << " in file " << fname2 << endl;
    return nhstbad = nbinbad = -4;
  }
  if ( m_pfile == 0 || !m_pfile->IsOpen() ) {
    cout << myname << "Output histogram file is not open." << endl;
    return nhstbad = nbinbad = -5;
  }
  int nx = ph1->GetNbinsX();
  int ny = ph1->GetNbinsY();
  if ( ph2->GetNbinsX() != nx || ph2->GetNbinsY() != ny ) {
    cout << myname << "Histograms have different binning." << endl;
    return nhstbad = nbinbad = -5;
  }
  m_pfile->cd();
  TH1* phd = dynamic_cast<TH1*>(ph1->Clone());
  phd->Add(ph2, -1.0);
  double maxfdiff = 0.0;
  for ( int iy=0; iy<=ny; ++iy ) {
    for ( int ix=0; ix<=nx; ++ix ) {
      float val1 = ph1->GetBinContent(ix+1, iy+1);
      float val2 = ph2->GetBinContent(ix+1, iy+1);
      double adiff = fabs(val1 - val2);
      double fdiff = 0.0;
      if ( adiff > 0.0 ) {
        double aavg = 0.5*fabs(val1 + val2);
        fdiff = adiff/aavg;
      }
      bool diff = fdiff > fdiffmax;
      bool show = false;
      if ( dbg == 1 ) show = diff!=0.0;
      else if ( dbg > 2 ) show = true;
      else if ( dbg == -1 ) show = ix == nx-1;
      else if ( dbg == -2 ) show = (ix == nx-1) && ( val1!=0.0 || val2!=0.0 );
      else {
        cout << "Invalid value for dbg: " << dbg << endl;
        return 99;
      }
      if ( show )
        cout << "  Bin " << ix << "-" << iy << ": " << val1 << " - " << val2
             << " = " << val1 - val2
             << " [ fdiff: " << fdiff << "]" << endl;
      if ( diff ) ++nbinbad;
      if ( fdiff > maxfdiff ) maxfdiff = fdiff;
      ++nbin;
      m_phdiff->Fill(val1-val2);
    }
  }
  if ( pmaxfdiff != nullptr ) *pmaxfdiff = maxfdiff;
  m_phdiff->Write();
  string oldtitle = phd->GetTitle();
  if ( oldtitle.substr(0,3) == "Raw" ) oldtitle = "r" + oldtitle.substr(1);
  string title = "Difference in " + oldtitle;
  phd->SetTitle(title.c_str());
  phd->SetMinimum(-20);
  phd->SetMaximum(20);
  phd->Write();
  m_pfile->Purge();
  ++nhst;
  if ( nbinbad ) {
    ++nhstbad;
    cout << "Histograms " << setw(16) << hname << " differ in " << setw(10) << nbinbad << "/" << setw(10) << nbin << " bins";
  } else {
    cout << "Histograms " << setw(16) << hname << "  match in " << setw(21) << nbin << " bins";
  }
  cout << " (" << fdiffmax << ")";
  cout << " max(fdiff)=" << maxfdiff;
  cout << endl;
  return nbinbad;
}

//**********************************************************************

int HistoCompare::compareall(string sdet, string hpre, int evt1, int evt2, double fdiffmax) {
  nbin = 0;
  nhst = 0;
  nbinbad = 0;
  nhstbad = 0;
  int nerr = 0;
  if ( evt1 > 0 ) {
    for ( int evt=evt1; evt<=evt2; ++evt ) {
      ostringstream sspre;
      sspre << "h" << evt << "_" << hpre;
      HistoCompare hc(fname1, fname2, dbg, m_pfile);
      hc.compareall(sdet, sspre.str(), 0, 0, fdiffmax);
      if ( hc.nbinbad >= 0 ) {
        nbin += hc.nbin;
        nhst += hc.nhst;
        nbinbad += hc.nbinbad;
        nhstbad += hc.nhstbad;
      } else {
        ++nerr;
      }
    }
    if ( nerr ) return nbinbad = nhstbad = -10*nerr;
    return nhstbad;
  }
  for ( string sapa : apas(sdet) ) {
    string hname = hpre + "apa" + sapa;
    HistoCompare hc(fname1, fname2, dbg, m_pfile);
    int icstat = hc.compare(hname, fdiffmax);
    if ( icstat < 0 ) {
      return nhstbad = icstat;
    }
    nhstbad += hc.nhstbad;
    nbinbad += hc.nbinbad;
    nbin += hc.nbin;
    ++nhst;
  }
  return nhstbad;
}

//**********************************************************************

void HistoCompare::print() const {
  cout << "HistoCompare configuration" << endl;
  cout << "  File 1: " << fname1 << endl;
  cout << "  File 2: " << fname2 << endl;
}

//**********************************************************************
