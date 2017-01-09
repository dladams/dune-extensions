// mycolors.h

#ifndef mycolors_H
#define mycolors_H

namespace {
  const unsigned int myncol = 8;
  int mycols[myncol] = {kBlue+2, kRed+1, kGreen+2, kOrange+3, kMagenta+1, kOrange-3, kViolet+2, 1};
  int myfcols[myncol] = {38, 46, 30, 28, 6, 42,  8, 12};
  int myblue   = mycols[0];
  int myred    = mycols[1];
  int mygreen  = mycols[2];
  int mybrown  = mycols[3];
  int mypink   = mycols[4];
  int myviolet = mycols[5];
  int myorange = mycols[6];
  int myblack  = mycols[7];
  int myyellow = kYellow - 9;
  int myltgray = 18;
  int mygray   = 17;
}

#endif
