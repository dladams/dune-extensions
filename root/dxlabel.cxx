// dxlabel.cxx

#include "dxlabel.h"

using std::string;

namespace {
string dxlab("undefined");
}

TLatex* dxlabel(string slab) {
  if ( slab.size() ) dxlab = slab;
  if ( dxlab == "undefined" ) return 0;
  TLatex* plab = new TLatex(0.01, 0.01, dxlab.c_str());
  plab->SetTextFont(160);
  plab->SetTextSize(0.04);
  plab->SetNDC();
  return plab;
}

