// test_RestrictedDFT.cxx

#undef NDEBUG

#include "RestrictedDFT.h"
#include <iostream>
#include <cassert>
#include "TH1.h"
#include "TCanvas.h"

using std::cout;
using std::endl;
using std::string;

int test_RestrictedDFT(int opt =0, unsigned int nfreq =3, unsigned int ntfit =200) {
  const string myname = "test_RestrictedDFT: ";
  cout << myname << "Create and plot DFT." << endl;
  vector<double> cs;
  cs.push_back(2.0);
  cs.push_back(1.0);
  cs.push_back(0.0);
  cs.push_back(0.0);
  cs.push_back(0.5);
  cs.push_back(0.0);
  cs.push_back(0.25);
  RestrictedDFT rd(50, 150, ntfit, cs);
  rd.print();
  new TCanvas;
  TH1* ph = rd.valueHist();
  ph->SetMarkerStyle(2);
  ph->Draw("p");
  cout << myname << "Fit and plot DFT." << endl;
  vector<double> data;
  if ( opt == 0 ) {
    rd.values(data);
  } else if ( opt == 1 ) {
    TH1* phd = dynamic_cast<TH1*>(ph->Clone("hdata"));
    double val = -8.0;
    for ( unsigned int it=0; it<150; ++it ) {
      phd->SetBinContent(it-50+1, val);
      data.push_back(val);
      val += 0.2;
    }
    phd->SetMarkerStyle(2);
    new TCanvas;
    phd->Draw("p");
  }
  cout << myname << "Data size: " << data.size() << endl;
  assert( data.size() > 0 );
  RestrictedDFT rdf(50, 150, ntfit, nfreq);
  rdf.fit(data);
  rdf.print();
  TH1* phf = rdf.valueHist("hfit", "Fitted");
  phf->Draw("same");
  return 0;
}
