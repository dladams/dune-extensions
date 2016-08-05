// dune_extensions/root/dxopen.cxx
//
// David Adams
// November 2015
//
// Root macro to open a DXDISPLAY file and move to the
// corresponding directory.

#include <string>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "dxopen.h"
#include "dxlabel.h"
#include "gettree.h"

using std::string;
using std::cout;
using std::endl;

int dxopen(string ifname);
int dxopen(TFile* pfile);
int dxopen(TObjArray* pobjs);
int dxopen();

// Open a file by name.
int dxopen(string ifname) {
  const string myname = "dxopen: ";
  // Do not attempt open if this looks like a macro call.
  if ( ifname.find("(") != string::npos ) {
    return 4;
  }
  TFile* pfile = TFile::Open(ifname.c_str());
  if ( pfile == 0 || ! pfile->IsOpen() ) {
    cout << myname << "Input file not found: " << ifname << endl;
    return 1;
  }
  cout << myname << "Opened file " << pfile->GetName() << endl;
  return dxopen(pfile);
}

// Initialize use of an open DXDISPLAY file.
int dxopen(TFile* pfile) {
  const string myname = "dxopen: ";
  if ( pfile == 0 ) return 2;
  pfile->cd();
  if ( ! pfile->cd("DXDisplay") ) {
    cout << myname << "Skipping non-DXDisplay file " << pfile->GetName() << endl;
    //pfile->Close();
    return 3;
  }
  gFile = pfile;
  if ( gDXFile != nullptr && gDXFile != pfile ) {
    cout << myname << "Closing DXDisplay file " << gDXFile->GetName() << endl;
    gDXFile->Close();
  }
  gDXFile = pfile;
  gDXFile->cd("DXDisplay");
  if ( mcptree("McParticleTree") ) mcptree()->SetMarkerStyle(2);
  if ( perftree("McPerfTree") ) perftree()->SetMarkerStyle(2);
  // Set the label name by stripping the directory path and suffix from the file name.
  string slab = pfile->GetName();
  string::size_type ipos1 = 0;
  string::size_type ipos = slab.rfind("/");
  if ( ipos != string::npos ) ipos1 = ipos + 1;
  ipos = slab.rfind(".");
  string::size_type npos = slab.size();
  if ( ipos != string::npos && ipos > ipos1 ) npos = ipos - ipos1;
  slab = slab.substr(ipos1, npos);
  if (slab.size() ) dxlabel(slab);
  return 0;
}

// Find a DXDISPLAY root file in an array of file names.
// Open it and remove that name from the array.
int dxopen(TObjArray* pobjs) {
  if ( pobjs == 0 ) return 4;
  for ( int iobj=0; iobj<pobjs->GetEntries(); ++iobj ) {
    TObject* pobj = pobjs->At(iobj);
    if ( pobj == 0 ) continue;
    TObjString* pos = dynamic_cast<TObjString*>(pobj);
    if ( pos == 0 ) continue;
    string fname(pos->GetString());
    int rstat = dxopen(fname);
    if ( rstat == 0 ) {
      pobjs->RemoveAt(iobj);
      
      return 0;
    }
  }
  return 0;
}

// Open the current DXDisplay file.
int dxopen() {
  const string myname = "dxopen: ";
  if ( gDXFile == 0 ) return 0;
  string fname = gDXFile->GetName();
  int rstat = dxopen(gDXFile);
  if ( rstat != 0 ) {
    cout << myname << "Unable to open DXDisplay file " << fname << endl;
    cout << myname << "Error " << rstat << "." << endl;
    return rstat;
  }
  cout << "DXDisplay file is " << gDXFile->GetName() << endl;
  return 0;
}
