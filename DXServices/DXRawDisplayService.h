// DXRawDisplayService.h

// David Adams
// February 2016
//
// This service creates and fills histograms of TPC raw data.
//
// Signal (i.e. channel-tick) histograms. These are recorded in eventE.
//   Notation:
//     E is the event ID
//     AAA is an APA identifier, e.g. 0u or 3z2
//   Histograms:
//     hE_rawapaAAA -    Raw data for APA AAA
//
// Properties:
//   LogLevel       - 0=no msgs, 1=init only, 2+=every call to process
//   TdcTickMin     - Lower limit for the tick axis.
//   TdcTickMax     - Upper limit for the tick axis.
//   NTickPerBin    - Rebin factor for the tick axis.
//   NChanPerBin    - Rebin factor for the channel axis.
//   DoROPs         - Make a channel-tick histogram for each readout plane for each event.
//   DoAll          - Make a channel-tick histogram for the full detector for each event.
//   DoAllOnline    - Make a channel-tick histogram for the full detector with online channel ordering for each event.
//   DoMean         - Make a channel mean +/- RMS histogram for each event.
//   NchanMeanRms   - if >0, make 2D channel vs. tick  mean and RMS histos with channel bin size of NchanMeanRms.
//   UseChannelMap  - Use service to convert offline to online channels (otherwise they are assumed to be the same)
//   BadChannelFlag - 0 - Use bad channels as they are
//                    1 - Set pedestal-subtracted ADC counts to zero
//   MaxEventsLog - Maximum # of events (calls to process) to log.
//   MaxDigitsLog - Maximum # of digits (calls to process) to log.

#ifndef DXRawDisplayService_H
#define DXRawDisplayService_H

#include <vector>
#include <string>

#include "DXInterface/RawDigitAnalysisService.h"

namespace fhicl {
  class ParameterSet;
}
namespace art {
  class ActivityRegistry;
  class Event;
}
namespace raw {
  class RawDigit;
}
namespace lariov {
  class DetPedestalProvider;
}
class TH2;
class TH1;
class GeoHelper;

class DXRawDisplayService : public RawDigitAnalysisService {

public:
 
  // Ctor from parameter set.
  explicit DXRawDisplayService(const fhicl::ParameterSet& pset);

  // Ctor for art.
  DXRawDisplayService(const fhicl::ParameterSet& pset, art::ActivityRegistry&);

  // Dtor.
  ~DXRawDisplayService();

  // The histogram filling routine, typically called once per event. 
  int process(const std::vector<raw::RawDigit>& digs, const art::Event* pevt) const override; 

private:

  // Display a line summarizing a 2D histogram.
  void summarize2dHist(TH2* ph, std::string prefix, unsigned int wnam,
                       unsigned int wbin, unsigned int went) const;

  // Delete event histograms after writing them to the output file.
  // This saves a lot of memory.
  void removeEventHists() const;

private:

  // Properties.
  int m_LogLevel;
  unsigned int m_TdcTickMin;
  unsigned int m_TdcTickMax;
  unsigned int m_NTickPerBin;
  unsigned int m_NChanPerBin;
  bool m_DoROPs;
  bool m_DoAll;
  bool m_DoAllOnline;
  bool m_DoMean;
  float m_NchanMeanRms;
  bool m_UseChannelMap;
  int m_BadChannelFlag;
  unsigned int m_MaxEventsLog;
  unsigned int m_MaxDigitsLog;

  GeoHelper* m_pgh;

  // Vector of event hists that should be removed at the end of the event.
  mutable std::vector<TH1*> m_eventhists;

  // counters;
  mutable unsigned int m_NEventsProcessed;
  mutable unsigned int m_NDigitsProcessed;

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(DXRawDisplayService, RawDigitAnalysisService, LEGACY)

#endif
