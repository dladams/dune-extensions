// SingleWireResponse.cxx

#include "SingleWireResponse.h"
#include "TH1F.h"
#include <iostream>

using std::cout;
using std::endl;

using Efield = SingleWireResponse::Efield;
//using Xy = SingleWireResponse::Xy;

//**********************************************************************

SingleWireResponse::SingleWireResponse(double arw, double arg) :
rw(arw), rg(arg) { }

//**********************************************************************

double SingleWireResponse::efield(double r) const {
  if ( r < rw ) return 0.0;
  if ( r > rg ) return 0.0;
  return vw/log(rg/rw)/r;
}

//**********************************************************************

Efield SingleWireResponse::efield(double x, double y) const {
  double r = sqrt(x*x + y*y);
  double er = efield(r);
  Efield ef;
  if ( r <= 0 ) return ef;
  ef.ex = er*x/r;
  ef.ey = er*y/r;
  ef.er = er;
  return ef;
}

//**********************************************************************

double SingleWireResponse::inducedCurrent(double x, double y, double vx, double vy) const {
  Efield ef = efield(x,y);
  return q*(ef.ex*vx + ef.ey*vy);
}

//**********************************************************************

double SingleWireResponse::integrateInducedCharge1(Xy xy1, Xy xy2, unsigned int nbin) const {
  double dx = xy2.x - xy1.x;
  double dy = xy2.y - xy1.y;
  double ds = sqrt(dx*dx+dy*dy);
  double dt = dx/vd;
  double vx = vd*dx/ds;
  double vy = vd*dy/ds;
  double ddx = dx/nbin;
  double ddy = dy/nbin;
  double ddt = dt/nbin;
  double sum = 0.0;
  double q = 0.5*inducedCurrent(xy1.x, xy1.y, vx, vy)*ddt;
  sum += q;
  q = 0.5*inducedCurrent(xy2.x, xy2.y, vx, vy)*ddt;
  sum += q;
  double x = xy1.x;
  double y = xy1.y;
  for ( unsigned int ibin=1; ibin<nbin; ++ibin ) {
    x += ddx;
    y += ddy;
    q = inducedCurrent(x, y, vx, vy)*ddt;
    sum += q;
  }
  return sum;
}

//**********************************************************************

double SingleWireResponse::integrateInducedCharge(Xy xy1, Xy xy2, unsigned int nbin) const {
  double dx = xy2.x - xy1.x;
  double dy = xy2.y - xy1.y;
  double ds = sqrt(dx*dx+dy*dy);
  double dt = dx/vd;
  double vx = vd*dx/ds;
  double vy = vd*dy/ds;
  double sum = 0.0;
  double ddx = dx/nbin;
  double ddy = dy/nbin;
  double ddt = dt/nbin;
  double x = xy1.x + 0.5*ddx;
  double y = xy1.y + 0.5*ddy;
  for ( unsigned int ibin=0; ibin<nbin; ++ibin ) {
    double q = inducedCurrent(x, y, vx, vy)*ddt;
    sum += q;
    x += ddx;
    y += ddy;
  }
  return sum;
}

//**********************************************************************

int SingleWireResponse::induce(Xy xy1, Xy xy2) {
  xyt.clear();
  eft.clear();
  respt.clear();
  qpt.clear();
  unsigned itic = 0;
  double x = xy1.x;
  double y = xy1.y;
  double dxtot = xy2.x - x;
  double dytot = xy2.y - y;
  double dstot = sqrt(dxtot*dxtot + dytot*dytot);
  double ds = ttick*vd;
  if ( ds < 1.e-6 ) return 1;
  double fac = ds/dstot;
  double dx = fac*dxtot;
  double dy = fac*dytot;
  x += 0.5*dx;
  y += 0.5*dy;
  double vx = vd*dx/ds;
  double vy = vd*dy/ds;
  double len = 0.0;
  DoubleVector out;
  double sumq = 0.0;
  double sumr = 0.0;
  while ( true ) {
    Efield ef = efield(x,y);
    double resp = q*(ef.ex*vx + ef.ey*vy)*ttick;
    xyt.push_back(Xy(x,y));
    eft.push_back(ef);
    Xy xy1(x-0.5*dx, y-0.5*dy);
    Xy xy2(x+0.5*dx, y+0.5*dy);
    double q = integrateInducedCharge(xy1, xy2, nbinint);
    qpt.push_back(q);
    respt.push_back(resp);
    sumq += q;
    sumr += resp;
    x += dx;
    y += dy;
    len += ds;
    if ( len > dstot + 0.01*ds ) break;
  }
  return 0;
}
    
//**********************************************************************

int SingleWireResponse::induce(double x1, double y1, double x2, double y2) {
  return induce(Xy(x1, y1), Xy(x2, y2));
}

//**********************************************************************

TH1* SingleWireResponse::resphist() {
  delete phresp;
  phresp = nullptr;
  unsigned ntick = qpt.size();
  if ( ntick == 0 ) return phresp;
  phresp = new TH1F("hresp", "Induced charge; Tick; I [fC/tick]", ntick, 0, ntick);
  phresp->SetStats(0);
  for ( unsigned int tick=0; tick<ntick; ++tick ) {
    phresp->SetBinContent(tick+1, qpt[tick]);
  }
  return phresp;
}

//**********************************************************************
