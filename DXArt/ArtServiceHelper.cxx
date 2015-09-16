// ArtServiceHelper.cxx

#include "DXArt/ArtServiceHelper.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceToken.h"
#include "art/Framework/EventProcessor/ServiceDirector.h"
#include "art/Framework/Services/System/TriggerNamesService.h"
#include "art/Framework/Services/System/CurrentModule.h"

using std::string;
using std::ostream;
using std::cout;
using std::endl;
using std::setw;
using std::find;
using std::unique_ptr;
using fhicl::ParameterSet;
using fhicl::make_ParameterSet;
using art::ServiceToken;
using art::ServiceDirector;
using art::ActivityRegistry;
using art::ServiceRegistry;
using art::TriggerNamesService;
using art::CurrentModule;

typedef ArtServiceHelper::NameList NameList;
typedef ArtServiceHelper::ConfigurationMap ConfigurationMap;

//**********************************************************************
// Local helpers.
//**********************************************************************

namespace {

// Return the art activity register.
ActivityRegistry& ar() {
  static unique_ptr<ActivityRegistry> par(new ActivityRegistry);
  return *par.get();
}

}  // end unnamed namespace

//**********************************************************************
// Public members.
//**********************************************************************

ArtServiceHelper& ArtServiceHelper::instance() {
  return *instancePtr().get();
}
  
//**********************************************************************

void ArtServiceHelper::close() {
  if ( instance().m_load == 3 ) return;
  delete instance().m_poperate;               // Close existing services and registry.
  instance().m_poperate = nullptr;            // Reset the old operatre.
  instancePtr().reset(new ArtServiceHelper);  // Delete the old service helper.
  instance().m_load = 3;                      // Put new instance in deleted state.
}
  
//**********************************************************************

int ArtServiceHelper::addService(string name, string scfg) {
  string myname = "ArtServiceHelper::addService: ";
  if ( m_load == 1 || m_load == 2 ) {
    cout << myname << "ERROR: Services may not be added after services are loaded." << endl;
    return 1;
  } else if ( m_load == 3 ) {
    cout << myname << "ERROR: Services may not be added after service hlper is closed." << endl;
    return 3;
  } else if ( m_load != 0 ) {
    cout << myname << "ERROR: Unexpected load status: " << m_load << "." << endl;
    return 4;
  }
  if ( find(m_names.begin(), m_names.end(), name) != m_names.end() ) {
    cout << myname << "ERROR: Service " << name << " is already registered" << endl;
    return 5;
  }
  if ( name == "TriggerNamesService" ) {
    cout << myname << "ERROR: TriggerNamesService is configured automatically." << endl;
    return 6;
  }
  if ( name == "CurrentModule" ) {
    cout << myname << "ERROR: CurrentModule service is configured automatically." << endl;
    return 7;
  }
  m_names.push_back(name);
  m_cfgmap[name] = scfg;
  if ( m_scfgs.size() ) m_scfgs += " ";
  m_scfgs += scfg;
  if ( name == "TFileService" ) m_needTriggerNamesService = true;
  if ( name == "RandomNumberGenerator" ) m_needCurrentModuleService = true;
  return 0;
}

//**********************************************************************

int ArtServiceHelper::loadServices() {
  string myname = "ArtServiceHelper::loadServices: ";
  if ( m_load == 1 ) {
    cout << myname << "ERROR: Services are already loaded." << endl;
    return 1;
  }
  if ( m_load == 2 ) {
    cout << myname << "ERROR: Service load failed earlier." << endl;
    return 2;
  }
  if ( m_load == 3 ) {
    cout << myname << "ERROR: Service manager is closed." << endl;
    return 2;
  }
  // Register standard services.
  ParameterSet cfgServices;
  make_ParameterSet(fullServiceConfiguration(), cfgServices);
  ServiceToken serviceToken;
  ServiceDirector director(cfgServices, ar(), serviceToken);
  // Register special services.
  if ( m_needTriggerNamesService ) {
    ParameterSet cfgTriggerNamesService;
    string scfgTriggerNamesService = "process_name: \"myproc\"";
    make_ParameterSet(scfgTriggerNamesService, cfgTriggerNamesService);
    std::vector<string> tns;
    unique_ptr<TriggerNamesService> psrv(new TriggerNamesService(cfgTriggerNamesService, tns));
    director.addSystemService(std::move(psrv));
  }
  if ( m_needCurrentModuleService ) {
    unique_ptr<CurrentModule> psrv(new CurrentModule(ar()));
    director.addSystemService(std::move(psrv));
  }
  // Make the services available
  // We need to keep this object around if we want to use the services.
  //// m_poperate.reset(new ServiceRegistry::Operate(serviceToken));
  m_poperate = new ServiceRegistry::Operate(serviceToken);
  return m_load = 1;
}

//**********************************************************************

NameList ArtServiceHelper::serviceNames() const {
  return m_names;
}

//**********************************************************************

string ArtServiceHelper::serviceConfiguration(std::string name) const {
  ConfigurationMap::const_iterator icfg = m_cfgmap.find(name);
  if ( icfg == m_cfgmap.end() ) return "";
  return icfg->second;
}

//**********************************************************************

string ArtServiceHelper::fullServiceConfiguration() const {
  return m_scfgs;
}

//**********************************************************************

int ArtServiceHelper::serviceStatus() const {
  return m_load;
}

//**********************************************************************

void ArtServiceHelper::print(ostream& out) const {
  if ( m_load == 3 ) {
    out << "Service manager is closed" << endl;
    return;
  }
  out << "ArtServiceHelper has " << m_names.size() << " "
      << (m_names.size()==1 ? "entry" : "entries")
      << (m_names.size()==0 ? "" : ":") << endl;
  unsigned int wnam = 8;
  for ( string name : m_names ) if ( name.size() > wnam ) wnam = name.size();
  for ( string name : m_names ) {
    out << setw(wnam+2) << name << ":: " << m_cfgmap.find(name)->second << endl;
  }
  if ( m_load == 0 ) out << "Services have not been loaded." << endl;
  else if ( m_load == 1 ) out << "Services have been loaded and are available." << endl;
  else if ( m_load == 2 ) out << "Service load failed." << endl;
  else out << "Unrecognized load status: " << m_load << "." << endl;
}

//**********************************************************************
// Private members.
//**********************************************************************

std::unique_ptr<ArtServiceHelper>& ArtServiceHelper::instancePtr() {
  static unique_ptr<ArtServiceHelper> psh(new ArtServiceHelper);
  return psh;
}
  
//**********************************************************************
