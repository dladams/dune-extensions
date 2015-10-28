// getLabel.cxx

#include "getLabel.h"
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::getline;

TLatex* getLabel(std::string fname, bool draw, int dbg) {
  const string myname = "getLabel: ";
  ifstream fin(fname.c_str());
  double xlab = 0.60;
  double ylab = 0.85;
  string slab;
  getline(fin, slab);
  if ( slab.size() == 0 ) {
    cout << myname << "ERROR: Unable to read title from file " << fname << endl;
    return nullptr;
  }
  fin >> xlab;
  fin >> ylab;
  if ( dbg ) {
    cout << myname << " File: " << fname << endl;
    cout << myname << "Label: " << slab << endl;
    cout << myname << "    X: " << xlab << endl;
    cout << myname << "    Y: " << ylab << endl;
  }
  TLatex* plab = new TLatex(xlab, ylab, slab.c_str());
  plab->SetNDC();
  plab->SetTextFont(42);
  if ( draw ) plab->Draw();
  return plab;
}
