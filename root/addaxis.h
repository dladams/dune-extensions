// addaxis.h
//
// David Adams
// November 2015
//
// Root macros to add top and/or right axis to the current pad.
//
// If histogram is specified, the axis attributes (tick size and
// # divisions) are taken from the histogram.
//
// After modifying plot range, repeat call to fix top/right axis.

#ifndef addaxis_H
#define addaxis_H

class TH1;

// Both.

// Draw top x-axis and right y-axis.
// If ph != 0, axis attributes are taken from the histogram.
int addaxis(TH1* ph =0);

// Top.

// Draw top x-axis with range taken from a histogram.
// If ph != 0, axis attributes are taken from the histogram.
int addaxistop(TH1* ph =0);

// Draw top x-axis with specified attributes.
int addaxistop(double ticksize, int ndiv);

// Right.

// Draw right y-axis with range taken from a histogram.
// If ph != 0, axis attributes are taken from the histogram.
int addaxisright(TH1* ph =0);

// Draw right y-axis with specified attributes.
int addaxisright(double ticksize, int ndiv);

#endif
