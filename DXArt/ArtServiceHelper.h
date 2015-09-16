// ArtServiceHelper.h

#ifndef ArtServiceHelper_H
#define ArtServiceHelper_H

// David Adams
// September 2015
//
// This class loads art services outside the art framework.
//
// Information about art services can be found at:
//   https://cdcvs.fnal.gov/redmine/projects/art/wiki/Guide_to_writing_and_using_services

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include "art/Framework/Services/Registry/ServiceRegistry.h"

class ArtServiceHelper {

public:

  typedef std::vector<std::string> NameList;
  typedef std::map<std::string, std::string> ConfigurationMap;

  // Return the one instance of this (singleton) class.
  static ArtServiceHelper& instance();

  // Close the one instance of this class.
  // Services are not longer available.
  // The current instance of this class and all services are deleted.
  // For TFileService, the files are written and renamed.
  // The load status is set to 3.
  static void close();

  // Dtor.
  ~ArtServiceHelper() = default;

  // Add a service.
  //   name - Name of the service, e.g. "TFileService"
  //   scfg - Configuration string for the service, e.g. for TFileService:
  //          TFileService: {service_type: "TFileService" fileName: "test.root"}
  // Configuration format is the same as that found in the services block of an fcl file.
  // Returns 0 for success.
  int addService(std::string name, std::string scfg);

  // Load the services, i.e. make them available for use via art::ServiceHandle.
  // Returns the status: 1 for success, 2 for failure.
  int loadServices();

  // Return the names of added services.
  NameList serviceNames() const;

  // Return the configuration string for a service.
  std::string serviceConfiguration(std::string name) const;

  // Return the full configuration string.
  std::string fullServiceConfiguration() const;

  // Return the service status.
  //   0 - not loaded
  //   1 - services loaded and available
  //   2 - service load failed
  //   3 - service helper is closed
  int serviceStatus() const;

  // Display the contents and status of a service helper.
  void print(std::ostream& out =std::cout) const;

private:

  // Return the pointer to the one instance of this (singleton) class.
  static std::unique_ptr<ArtServiceHelper>& instancePtr();

  // Ctors.
  ArtServiceHelper() = default;
  ArtServiceHelper(const ArtServiceHelper&) = delete;
  ArtServiceHelper& operator=(const ArtServiceHelper&) const = delete;

  NameList m_names;
  ConfigurationMap m_cfgmap;
  std::string m_scfgs;
  int m_load = 0;
  bool m_needTriggerNamesService = false;
  bool m_needCurrentModuleService = false;
  art::ServiceRegistry::Operate* m_poperate;

};

#endif
