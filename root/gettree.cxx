// gettree.cxx

#include "gettree.h"
#include <iostream>
#include <iomanip>
#include "TDirectory.h"
#include "TTree.h"

using std::string;
using std::cout;
using std::endl;
using std::hex;
using std::dec;

//**********************************************************************

TTree* gettree(string tname) {
  const string myname = "gettree: ";
  TTree* ptree = nullptr;
  if ( tname == "null" || tname == "" ) return nullptr;
  TObject* pobj = nullptr;
  gDirectory->GetObject(tname.c_str(), pobj);
  if ( pobj == nullptr ) {
    cout << myname << "Object " << tname << " not found in " << gDirectory->GetName() << endl;
    ptree = nullptr;
  } else {
    ptree = dynamic_cast<TTree*>(pobj);
    if ( pobj == nullptr ) {
      cout << myname << "Object " << tname << " is not a TTree." << endl;
    }
  }
  return ptree;
}

//**********************************************************************

TTree* mcptree(string tname) {
  const string myname = "mcptree: ";
  static TTree* ptree = nullptr;
  static string treename;
  if ( tname.size() && tname != treename ) {
    ptree = gettree(tname);
    treename = tname;
    cout << myname << "Tree " << treename << " found at " << hex << long(ptree) << dec << endl;
  }
  return ptree;
}

//**********************************************************************
