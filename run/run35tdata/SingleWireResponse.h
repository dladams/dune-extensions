// SingleWireResponse.h

#ifndef SingleWireResponse_H
#define SingleWireResponse_H

// Plot the response (induced current) from a charge moving 
// past a wire. Wire is at x = y =0.

class TH1;

class SingleWireResponse {

public:

  class Efield {
  public:
    Efield() : ex(0.0), ey(0.0), er(0.0) { }
    Efield(double aex, double aey) : ex(aex), ey(aey), er(sqrt(ex*ex+ey*ey)) { }
    double ex;
    double ey;
    double er;
  };

  class Xy {
    public:
    Xy() : x(0.0), y(0.0) { }
    Xy(double ax, double ay) : x(ax), y(ay) { }
    double x;
    double y;
  };

  using DoubleVector = std::vector<double>;
  using EfieldVector = std::vector<Efield>;
  using XyVector = std::vector<Xy>;

  // Ctor.
  // Wire and bounding radous in mm.
  SingleWireResponse(double arw =0.075, double arg =4.7);

  // Return the efield in V/mm.
  double efield(double r) const;
  Efield efield(double x,double y) const;

  // Return the induced current [fC/us] at (x, y) [mm]
  // for charge q [fC] moving with velocity (vx, vy) [mm/us].
  double inducedCurrent(double x, double y, double vx, double vy) const;

  // Return the the charge [fC] induced when for charge q [fC] drifts
  // with speed vd [mm/us] from xy1 to xy2 [mm].
  // Integration is performed over nbin bins.
  double integrateInducedCharge1(Xy xy1, Xy xy2, unsigned int nbin) const;
  double integrateInducedCharge(Xy xy1, Xy xy2, unsigned int nbin) const;

  // Induce charge along a straight-line trajectory.
  // tick is the tick size in us.
  // Results are in xyt, eft and respt
  int induce(Xy xy1, Xy xy2);
  int induce(double x1, double y1, double x2, double y2);

  // Return the histogram of the current reponse.
  TH1* resphist();

  // Data.
  double vw =1.0;     // Voltage on the wire
  double rw;          // wire radius
  double rg;          // radius of the grounded cylinder surrounding the wire
  double vd = 1.6;    // Drift speed [mm/us]
  double q = 6242;    // Charge in fC
  double ttick = 0.5; // Tick length in us
  unsigned int nbinint = 100;

  // Induced trajectory, field and current.
  XyVector xyt;
  EfieldVector eft;
  DoubleVector respt; // Integrated charge/tick using central field
  DoubleVector qpt;   // Integrated charge/tick using nbinint bins

  TH1* phresp = nullptr;

};

#endif
