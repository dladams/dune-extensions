// TriggerFilterService.h

// David Adams
// December 2016
//
// Service interface for analyzing TPC raw data.

#ifndef TriggerFilterService_H
#define TriggerFilterService_H

#include <vector>
#include <string>
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace raw {
  class ExternalTrigger;
}

class TriggerFilterService {
public:
 
  // Dtor.
  virtual ~TriggerFilterService() =default;

  // Select event. Return greater than zero to accept.
  virtual int keep(const std::vector<raw::ExternalTrigger>& trig) const =0;

};

DECLARE_ART_SERVICE_INTERFACE(TriggerFilterService, LEGACY)

#endif
