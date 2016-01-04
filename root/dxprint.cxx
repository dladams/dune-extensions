// dxprint.cxx

#include "dxprint.h"
#include "dxlabel.h"
#include "TCanvas.h"

using std::string;

namespace {
  string dxfname = "undefined";
  string dxfext = "png";
}

int dxprint(string fname, string fext, int act) {
  if ( fname.size() ) dxfname = fname;
  if ( fext.size() ) dxfext = fext;
  if ( dxfname == "undefined" ) return 1;
  string name = dxfname + "." + dxfext;
  if ( gPad == 0 ) return 2;
  if ( act == 2 ) {
    TLatex* plab = dxlabel();
    if ( plab != 0 ) plab->Draw();
  }
  if ( act==1 || act==2 ) {
    gPad->Print(name.c_str());
    return 0;
  }
  return 3;
}
