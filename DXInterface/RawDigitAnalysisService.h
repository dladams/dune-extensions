// RawDigitAnalysisService.h

// David Adams
// February 2016
//
// Service interface for analyzing TPC raw data.

#ifndef RawDigitAnalysisService_H
#define RawDigitAnalysisService_H

#include <vector>
#include <string>
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace fhiclcpp {
  class ParameterSet;
}
namespace art {
  class ActivityRegistry;
  class Event;
}
namespace raw {
  class RawDigit;
}
class TH2;
class TH1;

class RawDigitAnalysisService {
public:
 
  // Dtor.
  virtual ~RawDigitAnalysisService() =default;

  // Process the raw data.
  virtual int process(const std::vector<raw::RawDigit>& digs, const art::Event* pevt =nullptr) const =0;

};

DECLARE_ART_SERVICE_INTERFACE(RawDigitAnalysisService, LEGACY)

#endif
