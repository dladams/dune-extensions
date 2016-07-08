// RestrictedDFT.cxx

#include "RestrictedDFT.h"
#include <iostream>
#include "TH1F.h"
#include "TDecompChol.h"

using std::string;
using std::cout;
using std::endl;

typedef RestrictedDFT::Index Index;
typedef RestrictedDFT::Value Value;
typedef RestrictedDFT::Vector Vector;

//**********************************************************************

RestrictedDFT::RestrictedDFT(Index tmin, Index tmax, Index ntfit, Index nk)
: m_tmin(tmin),
  m_nt(tmax-tmin),
  m_ntfit(ntfit),
  m_coeffs((nk>0 ? 2*nk-1 : 0), 0.0),
  m_err(0.0), m_chsq(-1.0), m_dof(0) { }

//**********************************************************************

RestrictedDFT::RestrictedDFT(Index tmin, Index tmax, Index ntfit, const Vector& coeffs)
: m_tmin(tmin),
  m_nt(tmax-tmin),
  m_ntfit(ntfit),
  m_coeffs(coeffs),
  m_err(0.0), m_chsq(-1.0), m_dof(0) { }

//**********************************************************************

Index RestrictedDFT::nFrequency() const {
  return (m_coeffs.size() + 1)/2;
}

//**********************************************************************

Index RestrictedDFT::nCoefficient() const {
  return m_coeffs.size();
}
//**********************************************************************

void RestrictedDFT::print(int lev) const {
  cout << "RestrictedDFT" << endl;
  cout << "  tmin: " <<  tmin() << endl;
  cout << "  tmax: " <<  tmax() << endl;
  cout << "    nt: " << nTick() << endl;
  cout << " ntfit: " << nTickFit() << endl;
  cout << " nfreq: " << nFrequency() << endl;
  cout << " ncoff: " << nCoefficient() << endl;
  if ( lev >= 1 ) {
    cout << "  Coefficients:" << endl;
    for ( unsigned int ic=0; ic<m_coeffs.size(); ++ic ) {
      cout << "    " << ic << ": " << m_coeffs[ic] << endl;
    }
  }
  if ( m_err != 0.0 ) {
    cout << " Fit error: " << fitError() << endl;
    cout << "       DOF: " << fitDOF() << endl;
    cout << "      Chi2: " << fitChiSquare() << endl;
    cout << " Unw. Chi2: " << fitChiSquareUnweighted() << endl;
    cout << "  Chi2/DOF: " << fitChiSquarePerDOF() << endl;
    cout << " UChi2/DOF: " << fitChiSquareUnweighted()/fitDOF() << endl;
  }
}

//**********************************************************************

Value RestrictedDFT::termFunction(Index j, Index it) const {
  if ( j > m_coeffs.size() ) return 0.0;
  if ( j == 0 ) return 1.0;
  bool isodd = j%2;
  unsigned int k = isodd ? (j+1)/2 : j/2;
  const double twopi = 2.0*acos(-1.0);
  Value arg = twopi*k*(it)/m_ntfit;
  Value val = isodd ? cos(arg) : sin(arg);
  return val;
}

//**********************************************************************

Value RestrictedDFT::value(Index it) const {
  Value val = 0.0;
  for ( unsigned int ic=0; ic<m_coeffs.size(); ++ic ) {
    val += m_coeffs[ic]*termFunction(ic, it);
  }
  return val;
}

//**********************************************************************

int RestrictedDFT::values(Vector& vals) const {
  vals.resize(tmax(), 0.0);
  for ( unsigned int it=tmin(); it<tmax(); ++it ) {
    vals[it] = value(it);
  }
  return 0;
}

//**********************************************************************

TH1* RestrictedDFT::valueHist(string a_hname, string a_htitl) const {
  string hname = a_hname;
  string htitl = a_htitl;
  if ( hname == "" ) hname = "RestrictedDFT_values";
  if ( htitl == "" ) htitl = "Restricted DFT";
  TH1* ph = new TH1F(hname.c_str(), htitl.c_str(), nTick(), tmin(), tmax());
  string xlab = ph->GetXaxis()->GetTitle();
  if ( xlab.size() == 0 ) ph->GetXaxis()->SetTitle("Tick");
  string ylab = ph->GetYaxis()->GetTitle();
  if ( ylab.size() == 0 ) ph->GetYaxis()->SetTitle("Value");
  ph->SetStats(0);
  for ( unsigned int it=0; it<m_nt; ++it ) {
    Index tick = tmin() + it;
    ph->SetBinContent(it+1, value(tick));
  }
  return ph;
}

//**********************************************************************

int RestrictedDFT::fit(const Vector& valsin, Value err) {
  m_err = err;
  Vector errs(tmax(), err);
  BoolVector keep(tmax(), true);
  return doFit(valsin, errs, keep);
}

//**********************************************************************

int RestrictedDFT::fit(const Vector& valsin, const Vector& errsin) {
  m_err = -1.0;
  BoolVector keep(tmax(), true);
  return doFit(valsin, errsin, keep);
}

//**********************************************************************

int RestrictedDFT::fit(const TH1* hist, unsigned int tskipmin, unsigned int tskipmax) {
  BoolVector keep(tmax(), true);
  if ( tskipmax > tskipmin ) {
    for ( Index it=tskipmin; it!=tskipmax; ++it ) {
      keep[it] = false;
    }
  }
  return fit(*hist, &keep);
}

//**********************************************************************

int RestrictedDFT::fit(const TH1& hist, const BoolVector* pkeep) {
  const string myname = "RestrictedDFT::fit(TH1): ";
  Value thmin = hist.GetXaxis()->GetXmin();
  Value thmax = hist.GetXaxis()->GetXmax();
  Value nbin = hist.GetXaxis()->GetNbins();
  Value binw = hist.GetXaxis()->GetBinWidth(1);
  if ( binw != 1.0 ) {
    cout << myname << "ERROR: Fitted histogram bin width is " << binw << " instead of 1" << endl;
    return 11;
  }
  m_err = -1.0;
  Vector vals(tmax(), 0.0);
  Vector errs(tmax(), 0.0);
  BoolVector keep(tmax(), false);
  if ( pkeep != 0 ) keep = *pkeep;
  for ( Index it=tmin(); it<tmax(); ++it ) {
    int ibin = it - thmin + 1;
    if ( ibin <= 0 || ibin > nbin ) {
      cout << myname << "ERROR: Histogram does not cover the fit range." << endl;
      return 12;
    }
    vals[it] = hist.GetBinContent(ibin);
    errs[it] = hist.GetBinError(ibin);
    if ( keep[it] ) keep[it] = errs[it] > 0.0;
  }
  return doFit(vals, errs, keep);
}

//**********************************************************************

int RestrictedDFT::doFit(const Vector& valsin, const Vector& errsin, const BoolVector& keep) {
  string myname = "RestrictedDFT::doFit: ";
  int m_dbg = 0;
  if ( valsin.size() < tmax() ) return 1;
  // Extract measurement.
  Index npt = 0;
  TVectorD b(nTick());
  vector<int> iptTick(nTick(),999999);   // The tick for each fitted point.
  for ( unsigned int it=0; it<nTick(); ++it ) {
    if ( keep[it+tmin()] ) {
      b(npt) = valsin[it+tmin()];
      iptTick[npt] = it + tmin();
      ++npt;
    }
    //cout << b(it) << endl;
  }
  b.ResizeTo(npt);
  iptTick.resize(npt, 999999);
  // Check if there are enough points to do fit.
  Index ncof = m_coeffs.size();
  if ( npt < ncof ) {
    cout << "ERROR: " << "Too few points: (npt = " << npt << ") < (ncof = " << ncof << ")." << endl;
    return 1;
  }
  // Build error vector.
  TVectorD e(npt);
  for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
    Index it = iptTick[ipt];
    e(ipt) = errsin[it];
  }
  if ( m_dbg  >= 3 ) {
    for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
      Index it = iptTick[ipt];
      cout << myname << it << ": " << b(ipt) << " +/- " << e(ipt) << endl;
    }
  }
  // Build coefficient matrix.
  TMatrixD m(npt, ncof);
  for ( unsigned int ic=0; ic<ncof; ++ic ) {
    for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
      Index it = iptTick[ipt];
      m(ipt, ic) = termFunction(ic, it);
    }
  }
  if ( m_dbg >= 4 ) {
    cout << myname << "Coefficient matrix:" << endl;
    for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
      Index it = iptTick[ipt];
      cout << myname << it << ":";
      for ( unsigned int ic=0; ic<ncof; ++ic ) {
        cout << " " << m(ipt, ic);
      }
      cout << endl;
    }
  }
  // Solve the equation.
  TVectorD newcofs = NormalEqn(m, b, e);
  if ( ! newcofs.IsValid() ) {
    cout << myname << "ERROR: Fit failed." << endl;
    m_err = -99;
    return 2;
  }
  for ( unsigned int ic=0; ic<ncof; ++ic ) {
    m_coeffs[ic] = newcofs[ic];
  }
  // Evaluate chi-square.
  m_chsqunw = 0.0;
  m_chsq = 0.0;
  for ( unsigned int ipt=0; ipt<npt; ++ipt ) {
    Index it = iptTick[ipt];
    Value val = value(it);
    Value msd = valsin[it];
    Value dif = msd - val;
    Value err = errsin[it];
    m_chsqunw += dif*dif;
    m_chsq += dif*dif/(err*err);
    if ( m_dbg >= 5 ) {
      cout << myname << it << ": " << val << " - " << msd << " = " << dif << endl;
    }
  }
  m_dof = npt - ncof;
  return 0;
}

//**********************************************************************
