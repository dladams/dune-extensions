// ApaName.h

//
// David Adams
// January 2017
//
// Class to convert APA names to ROP ID's.

#ifndef ApaName_H
#define ApaName_H

#include <string>
#include <vector>
#include "larcoreobj/SimpleTypesAndConstants/readout_types.h"

class ApaName {

public:

  typedef unsigned int Index;
  typedef readout::ROPID ROPID;
  typedef std::string Name;
  typedef std::vector<Name> NameVector;

  // Value indicating an invalid index.
  Index badIndex() const { return 999999; }

  // Names for the planes.
  NameVector planeNames() const;
  Name planeName(Index ipla) const;

  // Return the name for an APA.
  Name name(readout::ROPID ropid) const;
  Name name(Index icry, Index iapa, Index ipla) const;
  Name name(Index iapa, Index ipla) const;

  // Return the ROPID for an APA.
  ROPID id(Name name) const;
  ROPID id(Index icry, Index iapa, Index ipla) const;
  ROPID id(Index iapa, Index ipla) const;

private:

  std::vector<std::string> m_planeNames = {"u", "v", "z1", "z2"};

};

#endif
