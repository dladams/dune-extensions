// TruncatedHist.h

#ifndef TruncatedHist_H
#define TruncatedHist_H

// Clones a histogram and sets it range to encompass
// (mean-nrms*RMS, mean+nrms*RMS).

class TH1;

class TruncatedHist {

public:

  // Ctor.
  TruncatedHist(TH1* ph, double a_nrms, bool dbg =false);

  // Dtor.
  // Deletes the tuncated histogram.
  ~TruncatedHist();

  // Return nrms.
  double nrms() const;

  // Return the truncated histogram.
  TH1* hist() const;

  // Return the # iteration used in the truncation.
  int nloop() const;

private:

  TH1* m_ph;
  double m_nrms;
  int m_nloop =0;

};

#endif
