// fix2dcanvas.C
//
// David Adams
// November 2015
//
// Root macro that sets the current canvas drawing frame
// background color to the first color in the palette.
// This fixed COLZ plots made in Root 5.34.

void fix2dcanvas() {
  gPad->SetFrameFillColor(gStyle->GetColorPalette(0));
}
