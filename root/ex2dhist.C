// ex2dhist.C
//
// Example showing drawing of a 2D histogram.
//
// I would like the lower left to be filled.

#include "fix2dcanvas.h"
#include "addaxis.h"

int ex2dhist() {
  TH2* ph2= new TH2F("h2", "z = x + y;x;y", 200, -10, 10, 200, -10, 10);
  for ( int j=-100; j<100; ++j ) {
    for ( int i=-100; i<100; ++i ) {
      double x = 0.1*i + 0.05;
      double y = 0.1*j + 0.05;
      double val = x + y;
      ph2->Fill(x, y, val);
    }
  }
  ph2->SetContour(40);
  ph2->SetStats(0);
  ph2->SetMinimum(-10);
  ph2->SetMaximum( 10);
  new TCanvas("colz1", "colz1", 700, 700);
  fix2dcanvas();  // Needed in Root 5.34.
  double xmax = 7;
  ph2->GetXaxis()->SetRangeUser(-xmax, xmax);
  ph2->GetYaxis()->SetRangeUser(-xmax, xmax);
  ph2->GetYaxis()->SetNdivisions(410);
  ph2->SetTickLength(0.01, "X");
  ph2->SetTickLength(0.02, "Y");
  ph2->Draw("colz1");
  addaxis(ph2);  // Adds top and right axes (w/o labels)
  return 0;
}
