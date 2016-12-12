// DXTriggerFilterService_service.cc

#include "DXTriggerFilterService.h"

#include <sstream>


// Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "cetlib/exception.h"

// Larsoft includes.
#include "lardataobj/RawData/ExternalTrigger.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ostringstream;

using raw::ExternalTrigger;

//************************************************************************

DXTriggerFilterService::DXTriggerFilterService(const fhicl::ParameterSet& pset)
: m_LogLevel(1) {
  const string myname = "DXTriggerFilterService::ctor: ";
  if ( m_LogLevel > 0 ) cout << myname << "Begin job." << endl;

  pset.get_if_present<int>("LogLevel", m_LogLevel);
  m_Triggers = pset.get<vector<unsigned int>>("Triggers");
  if ( m_LogLevel >= 1 ) {
    cout << myname << "LogLevel: " << m_LogLevel << endl;
    cout << myname << "Triggers: [";
    bool prefix = "";
    for ( unsigned int trig : m_Triggers ) {
      cout << prefix << trig;
      prefix = ", ";
    }
    cout << "]" << endl;
  }
}

//************************************************************************

DXTriggerFilterService::DXTriggerFilterService(const fhicl::ParameterSet& pset, art::ActivityRegistry&)
: DXTriggerFilterService(pset) { }

//************************************************************************

DXTriggerFilterService::~DXTriggerFilterService() {
  const string myname = "DXTriggerFilterService::dtor: ";
  if ( m_LogLevel >= 1 ) {
    cout << myname << "   # kept events: " << m_keep << endl;
    cout << myname << "# dropped events: " << m_skip << endl;
  }
}
   
//************************************************************************

int DXTriggerFilterService::keep(const std::vector<raw::ExternalTrigger>& extrigs) const {
  const string myname = "DXTriggerFilterService::keep: ";
  for ( unsigned int trig : m_Triggers ) {
    if ( extrigs.size() ) {
      for ( const ExternalTrigger& extrig : extrigs ) {
        unsigned int trigin = extrig.GetTrigID();
        if ( trigin == trig ) {
          ++m_keep;
          if ( m_LogLevel >= 2 ) cout << myname << "Keeping trigger " << trig << endl;
          return 1;
        } else {
          if ( m_LogLevel >= 3 ) cout << myname << "Rejecting trigger " << trigin << endl;
        }
      }
    } else {
      if ( m_LogLevel >= 3 ) cout << myname << "No input triggers." << endl;
    }
  }
  if ( m_LogLevel >= 2 ) cout << myname << "Skipped trigger." << endl;
  ++m_skip;
  return 0;
}

//************************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(DXTriggerFilterService, TriggerFilterService)

//**********************************************************************
