// TruncatedHist.h

#ifndef TruncatedHist_H
#define TruncatedHist_H

// Clones a histogram and sets it range to encompass
// (mean-nrms*RMS, mean+nrms*RMS).

class TH1;

class TruncatedHist {

public:

  // Ctor.
  TruncatedHist(TH1* ph, double a_nrms);

  // Dtor.
  // Deletes the tuncated histogram.
  ~TruncatedHist();

  // Return nrms.
  double nrms() const;

  // Return the truncated histogram.
  TH1* hist;

private:

  TH1* m_ph;
  double m_nrms;

};

#endif
