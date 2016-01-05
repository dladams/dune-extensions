// dxprint.cxx

#include "dxprint.h"
#include <iostream>
#include "dxlabel.h"
#include "TCanvas.h"

using std::string;
using std::cout;
using std::endl;

namespace {
  string dxfname = "undefined";
  string dxfext = "png";
}

int dxprint(string fname, string fext, int act) {
  if ( fname.size() ) dxfname = fname;
  if ( fext.size() ) dxfext = fext;
  if ( dxfname == "undefined" ) {
    cout << "Set plot label with dxprint(LABEL)" << endl;
    return 1;
  }
  string name = dxfname + "." + dxfext;
  if ( gPad == 0 ) return 2;
  if ( act == 2 ) {
    // Add label to plot.
    TLatex* plab = dxlabel();
    if ( plab != 0 ) plab->Draw();
  }
  if ( act==1 || act==2 ) {
    gPad->Print(name.c_str());
    return 0;
  }
  return 3;
}
