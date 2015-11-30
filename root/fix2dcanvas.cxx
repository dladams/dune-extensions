// fix2dcanvas.cxx

#include "fix2dcanvas.h"
#include "TPad.h"
#include "TStyle.h"

void fix2dcanvas() {
  gPad->SetFrameFillColor(gStyle->GetColorPalette(0));
}
