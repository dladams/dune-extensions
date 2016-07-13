// RestrictedDFT.h

#ifndef RestrictedDFT_H
#define RestrictedDFT_H

// David Adams
// July 2016
//
// This class models a discrete time spectrum using the lowest nk
// frequencies in a DFT (discrete Fourier transform) with period ntfit.
// The spectrum has values for the nt ticks
//   {tmin, tmin+1, ..., tmax-1}.
// The model has 2*nk-1 free parameters implying that we must have
//   nk < (nt+1)/2
// to have more measurments than paramters.
//
// The model value for bin it is
//   value(it) = SUM_j C_j F_j_it
// where C_j is the term coefficient and F_j_it is the term function value:
//   F_0_it = 1
//   F_j_it = cos(2pi*(j+1)/2*(it)/ntfit) for j odd
//   F_j_it = sin(2pi*j/2*(it)/ntfit) for j>0 even
// with j = 0, 1, .. 2*nk
// Note this form implies the fit function is periodic with period ntfit.
//
// The tick range [tmin, tmax) is used for fitting and when the fitted
// values are returned as a vector or histogram. A conventional DFT
// would start at 0 and have ntfit = nt, i.e. cover the range [0, ntfit).
//
// Choosing ntfit > nt (nt = tmax -tmin) implies`

#include <string>
#include <vector>
#include <cmath>

class TH1;

class RestrictedDFT {

public:

  typedef unsigned int Index;
  typedef double Value;
  typedef std::vector<Value> Vector;
  typedef std::vector<bool> BoolVector;

  // Ctor from t0, # points and # frequencies.
  // The range of ticks is [tmin, tmax).
  // The term frequencies are 1/fit, 2/ntfit, ...
  RestrictedDFT(Index tmin, Index tmax, Index ntfit, Index nk, Index fitopt);

  // Ctor from t0, # points array of coefficients.
  RestrictedDFT(Index tmin, Index tmax, Index ntfit, const Vector& coeffs);

  // Return the time range.
  Index tmin() const { return m_tmin; }
  Index tmax() const { return m_tmin + m_nt; }
  Index nTick() const { return m_nt; }

  // Return the fit range.
  Index nTickFit() const { return m_ntfit; }

  // Return the number of frequencies.
  Index nFrequency() const;

  // Return the fit option.
  Index fitOption() const { return m_fitopt; }

  // Return the number of coefficients.
  Index nCoefficient() const;

  // Return the coefficients.
  const Vector& coefficients() const { return m_coeffs; }

  // Display this object including coefficients.
  void print(int lev =0) const;

  // Return the term function value for term j and tick it.
  Value termFunction(Index j, Index it) const;

  // Return the value for bin it (< nt).
  Value value(Index it) const;

  // Return the values for all bins.
  int values(Vector& vals) const;

  // Fill a histogram with the values.
  TH1* valueHist(std::string hname ="", std::string htitl ="") const;

  // Fit some data.
  int fit(const Vector& valsin, Value err =1.0/sqrt(12.0));
  int fit(const Vector& valsin, const Vector& errsin);
  int fit(const TH1* hist, unsigned int tskipmin =1, unsigned int tskipmax =0);
  int fit(const TH1& hist, const BoolVector* pkeep =0);

  // Return the error assumed in the fit.
  // Zero means no fit has been performed.
  Value fitError() const { return m_err; }

  // Return chi-square from fit.
  Value fitChiSquare() const { return m_chsq; }

  // Return chi-square from fit.
  Value fitChiSquareUnweighted() const { return m_chsqunw; }

  // Return the # degrees of freedom (n_meas - n_coeff) for the fit.
  Value fitDOF() const { return m_dof; }

  // Return chi-square from fit.
  Value fitChiSquarePerDOF() const { return m_err!=0.0 && m_dof>0.0 ? m_chsq/m_dof : -1.0; }

private:

  int doFit(const Vector& valsin, const Vector& errsin, const BoolVector& keep);
  int lsfFit(const Vector& valsin, const Vector& errsin, const BoolVector& keep);
  int progFit(const Vector& valsin, const Vector& errsin, const BoolVector& keep, bool update, Index npass);

  Index m_tmin;          // First time bin.
  Index m_nt;            // # time bins.
  Index m_ntfit;         // # ticks in the fit DFT; model period
  Index m_fitopt;        // Fitting option.
  Vector m_coeffs;       // Array of term coefficients.
  Value m_err;           // Uncertainty assumed in the fit.
  Value m_chsqunw;       // Unweighted fit chi-square.
  Value m_chsq;          // Fit chi-square.
  Index m_dof;           // # degrees of freedom in fit.

};

#endif
