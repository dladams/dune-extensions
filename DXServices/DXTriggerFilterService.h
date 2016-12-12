// DXTriggerFilterService.h

// David Adams
// December 2016
//
// This service selects events based on trigger data.
// An event is accepted if any ExternalTrigger entry has an ID matching
// any entry in the parameter list of trigger IDs.
//
// Parameters.
//   LogLevel: 0=quiet, 1=init only, >1=every call
//   Triggers: Trigger IDs to be accepted

#ifndef DXTriggerFilterService_H
#define DXTriggerFilterService_H

#include <vector>
#include <string>

#include "DXInterface/TriggerFilterService.h"

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

class DXTriggerFilterService : public TriggerFilterService {

public:
 
  // Ctor from parameter set.
  explicit DXTriggerFilterService(const fhicl::ParameterSet& pset);

  // Ctor for art.
  DXTriggerFilterService(const fhicl::ParameterSet& pset, art::ActivityRegistry&);

  // Dtor.
  ~DXTriggerFilterService();

  // The filter.
  int keep(const std::vector<raw::ExternalTrigger>& trig) const override;

private:

  // Properties.
  int m_LogLevel;
  std::vector<unsigned int> m_Triggers;

  // Counters.
  mutable unsigned int m_keep =0;
  mutable unsigned int m_skip =0;

};

DECLARE_ART_SERVICE_INTERFACE_IMPL(DXTriggerFilterService, TriggerFilterService, LEGACY)

#endif
