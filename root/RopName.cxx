// RopName.cxx

#include "RopName.h"
#include <sstream>

using std::ostringstream;
using std::istringstream;

typedef RopName::Index Index;
typedef RopName::Name Name;
typedef RopName::NameVector NameVector;
typedef RopName::ROPID ROPID;

//**********************************************************************

NameVector RopName::planeNames() const {
  return m_planeNames;
}
//**********************************************************************

Name RopName::planeName(Index ipla) const {
  if ( ipla >= m_planeNames.size() ) return "InvalidPlane";
  return m_planeNames[ipla];
}

//**********************************************************************

Name RopName::name(readout::ROPID ropid) const {
  return name(ropid.Cryostat, ropid.TPCset, ropid.ROP);
}

//**********************************************************************

Name RopName::name(Index icry, Index iapa, Index ipla) const {
  if ( ipla >= m_planeNames.size() ) {
    return "InvalidAPA";
  }
  ostringstream ssname;
  ssname << "apa";
  if ( icry > 0 ) ssname << icry << "-";
  ssname << iapa;
  ssname << m_planeNames[ipla];
  return ssname.str();
}

//**********************************************************************

Name RopName::name(Index iapa, Index ipla) const {
  return name(0, iapa, ipla);
}

//**********************************************************************

ROPID RopName::id(Name ropname) const {
  Index icry = 0;
  Index iapa = badIndex();
  Index ipla = badIndex();
  if ( ropname.substr(0, 3) == "apa" ) {
    Name rem = ropname.substr(3);
    Name::size_type ipos = rem.find("-");
    if ( ipos != Name::npos && ipos > 0 ) {
      istringstream sscry(rem.substr(0, ipos));
      sscry >> icry;
    }
    rem = rem.substr(ipos+1);
    if ( rem.size() > 1 ) {
      Name npl;
      for ( Index jpla=0; jpla<m_planeNames.size(); ++jpla ) {
        npl = m_planeNames[ipla];
        if ( rem.substr(rem.size() - npl.size()) == npl ) ipla = jpla;
      }
      if ( ipla != badIndex() ) {
        istringstream ssapa(rem.substr(0, rem.size() - npl.size()));
        ssapa >> iapa;
      }
    }
  }
  return ROPID(icry, iapa, ipla);
}

//**********************************************************************

ROPID RopName::id(Index icry, Index iapa, Index ipla) const {
  return ROPID(icry, iapa, ipla);
}

//**********************************************************************

ROPID RopName::id(Index iapa, Index ipla) const {
  return ROPID(0, iapa, ipla);
}

//**********************************************************************
