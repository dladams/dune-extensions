// addaxis.cxx

#include "addaxis.h"
#include "TH1.h"
#include "TPad.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TList.h"

//**********************************************************************

int addaxis(TH1* ph) {
  addaxistop(ph);
  return addaxisright(ph);
}

//**********************************************************************

int addaxistop(TH1* ph) {
  double ticksize = 0;
  int ndiv = 0;
  if ( ph != 0 ) {
    TAxis* paxold = ph->GetXaxis();
    if ( paxold != 0 ) {
      ticksize = paxold->GetTickLength();
      ndiv = paxold->GetNdivisions();
    }
  }
  return addaxistop(ticksize, ndiv);
}

//**********************************************************************

int addaxistop(double ticksize, int ndiv) {
  double xmin = 0.0;
  double xmax = 0.0;
  double ymin = 0.0;
  double ymax = 0.0;
  if ( gPad == 0 ) return 3;
  gPad->Update();
  gPad->GetRangeAxis(xmin, ymin, xmax, ymax);
  TGaxis* paxnew = new TGaxis( gPad->GetUxmin(), gPad->GetUymax(),
                               gPad->GetUxmax(), gPad->GetUymax(),
                               xmin, xmax, 510, "-US");
  if ( ticksize > 0 ) paxnew->SetTickSize(ticksize);
  if ( ndiv > 0 ) paxnew->SetNdivisions(ndiv);
  string name = "TopAxis";
  paxnew->SetName(name.c_str());
  TList* pobjs = gPad->GetListOfPrimitives();
  for ( int iobj=0; iobj<pobjs->GetEntries(); ++iobj ) {
    TGaxis* paxold = dynamic_cast<TGaxis*>(pobjs->At(iobj));
    if ( paxold != 0 ) {
      
      if ( paxold->GetName() == name ) {
        pobjs->RemoveAt(iobj);
        break;
      }
    }
  }
  paxnew->Draw();
  return 0;
}

//**********************************************************************

int addaxisright(TH1* ph) {
  double ticksize = 0;
  int ndiv = 0;
  if ( ph != 0 ) {
    TAxis* paxold = ph->GetYaxis();
    if ( paxold != 0 ) {
      ticksize = paxold->GetTickLength();
      ndiv = paxold->GetNdivisions();
    }
  }
  return addaxisright(ticksize, ndiv);
}

//**********************************************************************

int addaxisright(double ticksize, int ndiv) {
  double xmin = 0.0;
  double xmax = 0.0;
  double ymin = 0.0;
  double ymax = 0.0;
  if ( gPad == 0 ) return 3;
  gPad->Update();
  gPad->GetRangeAxis(xmin, ymin, xmax, ymax);
  TGaxis* paxnew = new TGaxis( gPad->GetUxmax(), gPad->GetUymin(),
                               gPad->GetUxmax(), gPad->GetUymax(),
                               ymin, ymax, 510, "+US");
  if ( ticksize > 0 ) paxnew->SetTickSize(ticksize);
  if ( ndiv > 0 ) paxnew->SetNdivisions(ndiv);
  string name = "RightAxis";
  paxnew->SetName(name.c_str());
  TList* pobjs = gPad->GetListOfPrimitives();
  for ( int iobj=0; iobj<pobjs->GetEntries(); ++iobj ) {
    TGaxis* paxold = dynamic_cast<TGaxis*>(pobjs->At(iobj));
    if ( paxold != 0 ) {
      
      if ( paxold->GetName() == name ) {
        pobjs->RemoveAt(iobj);
        break;
      }
    }
  }
  paxnew->Draw();
  return 0;
}

//**********************************************************************
