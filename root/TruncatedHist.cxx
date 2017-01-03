// TruncatedHist.cxx

#include "TruncatedHist.h"
#include "TH1.h"
#include <iostream>

using std::abs;

//**********************************************************************

TruncatedHist::TruncatedHist(TH1* ph, double a_nrms)
: m_ph(nullptr), m_nrms(a_nrms), m_nloop(0) {
  m_ph = dynamic_cast<TH1*>(ph->Clone());
  m_ph->SetDirectory(0);
  double dif = 1;
  while ( ++m_nloop < 20 ) {
    double mean = ph->GetMean();
    double rms = ph->GetRMS();
    double x1 = mean - m_nrms*rms;
    double x2 = mean + m_nrms*rms;
    m_ph->GetXaxis()->SetRangeUser(x1,x2);
    double drmsmax = 1.e-4*(x2 - x1);
    double newrms = ph->GetRMS();
    double drms = std::abs(newrms - rms);
    if ( drms < drmsmax ) break;
  }
}

//**********************************************************************

TruncatedHist::~TruncatedHist() {
  delete m_ph;
}

//**********************************************************************

double TruncatedHist::nrms() const {
  return m_nrms;
}

//**********************************************************************

TH1* TruncatedHist::hist() const {
  return m_ph;
}

//**********************************************************************

int TruncatedHist::nloop() const {
  return m_nloop;
}

//**********************************************************************
