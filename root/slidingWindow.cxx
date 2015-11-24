// slidingWindow.cxx

#include <string>
#include <sstream>
#include <iostream>
#include <TH2.h>

using std::string;
using std::cout;
using std::endl;
using std::ostringstream;

//   phin - pointer to the input histogram
//    nwx - sliding window size: bins i to i+nwx are summed into i
// useabs - store the abs for the above sum

TH2* slidingWindow(TH2* phin, double ts, unsigned int nwx, int useabs) {
  const string myname = "slidingWindow: ";
  unsigned int nx = phin->GetNbinsX();
  unsigned int ny = phin->GetNbinsY();
  string name = phin->GetName();
  name += "sw";
  ostringstream sstitle;
  sstitle << phin->GetTitle() << " TS=" << ts << " NS=" << nwx;
  string title = sstitle.str();
  string::size_type ipos = title.find("Raw");
  if ( ipos == string::npos ) {
    cout << myname << "Input histogram must be raw." << endl;
    return 0;
  }
  title.replace(ipos, 3, "SW");
  TH2* ph = dynamic_cast<TH2*>(phin->Clone(name.c_str()));
  ph->SetTitle(title.c_str());
  if ( useabs ) ph->SetMinimum(0.0);
  for ( unsigned int iy=1; iy<=ny; ++iy ) {
    for ( unsigned int ix=1; ix<=nx; ++ix ) {
      double sum = 0.0;
      if ( iy==0 || ix==0 || iy==ny+1 || ix==nx+1 ) {
        sum = 0.0;
      } else {
        unsigned int jx1 = ix;
        unsigned int jx2 = ix + nwx - 1;
        if ( jx2 > nx ) jx2 = nx;
        for ( unsigned int jx=jx1; jx<=jx2; ++jx ) {
          double sig = phin->GetBinContent(jx, iy);
          if ( fabs(sig) > 3.0 ) sum += sig;
        }
        double nwxact = jx2 - jx1 + 1;
        sum /= nwxact;
        if ( useabs ) sum = fabs(sum);
      }
      ph->SetBinContent(ix, iy, sum);
    }
  }
  return ph;
}

TH2* deadWindow(TH2* phin, double td, unsigned int nd) {
  const string myname = "deadWindow: ";
  unsigned int nx = phin->GetNbinsX();
  unsigned int ny = phin->GetNbinsY();
  string name = phin->GetName();
  name += "~dead";
  ostringstream sstitle;
  sstitle << phin->GetTitle() << " TD=" << td << " ND=" << nd;
  string title = sstitle.str();
  string::size_type ipos = title.find("SW signals");
  if ( ipos == string::npos ) {
    cout << myname << "Input histogram must be sliding window." << endl;
    return 0;
  }
  title.replace(ipos, 10, "Live regions");
  TH2* ph = dynamic_cast<TH2*>(phin->Clone(name.c_str()));
  ph->SetTitle(title.c_str());
  for ( unsigned int iy=1; iy<=ny; ++iy ) {
    for ( unsigned int ix=1; ix<=nx; ++ix ) {
      double sum = 0.0;
      if ( iy==0 || ix==0 || iy==ny+1 || ix==nx+1 ) {
        sum = 0.0;
      } else {
        unsigned int jx1 = 0;
        if ( nd + 1 < ix ) jx1 = ix - nd + 1;
        unsigned int jx2 = ix;
        sum = 0;
        for ( unsigned int jx=jx1; jx<=jx2; ++jx ) {
          if ( phin->GetBinContent(jx, iy) > td ) sum = 1.0;
        }
      }
      ph->SetBinContent(ix, iy, sum);
    }
  }
  ph->SetContour(20);
  ph->SetMinimum(0.0);
  ph->SetMaximum(1.0);
  return ph;
}
