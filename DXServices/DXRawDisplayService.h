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
//   LogLevel    - 0=no msgs, 1=init only, 2+=every call to process
//   TdcTickMin  - Lower limit for the tick axis.
//   TdcTickMax  - Upper limit for the tick axis.
//   NTickPerBin - Rebin factor for the tick axis.
//   NChanPerBin - Rebin factor for the channel axis.
//   DoROPs      - Make a histogram for each readout plane.
//   DoAll       - Make a histogram for the full detector.
//   DoAllOnline - Make a histogram for the full detector with online channel ordering.
//   PedestalOption - 1=take from digit, 2=take from service, otherwise none
//   AdcOffset   - Offset added to all bins.
//   DecompressWithPedestal - Include pedestal when decompressing data.

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
  int process(std::vector<raw::RawDigit>& digs, const art::Event* pevt) const override; 

private:

  // Display a line summarizing a 2D histogram.
  void summarize2dHist(TH2* ph, std::string prefix, unsigned int wnam,
                       unsigned int wbin, unsigned int went) const;

  // Delete event histograms after writing them to the output file.
  // This saves a lot of memory.
  void removeEventHists() const;

private:

  // Properties.
  int m_dbg;
  unsigned int m_TdcTickMin;
  unsigned int m_TdcTickMax;
  unsigned int m_NTickPerBin;
  unsigned int m_NChanPerBin;
  bool m_DoROPs;
  bool m_DoAll;
  bool m_DoAllOnline;
  int m_PedestalOption;
  float m_AdcOffset;
  bool m_DecompressWithPedestal;

  const lariov::DetPedestalProvider* m_pPedProv;

  GeoHelper* m_pgh;

  // Vector of event hists that should be removed at the end of the event.
  mutable std::vector<TH1*> m_eventhists;

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(DXRawDisplayService, RawDigitAnalysisService, LEGACY)

#endif
