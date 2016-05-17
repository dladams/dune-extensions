// DrawResult.h

// David Adams
// January 2016
//
// Struct to hold the result of a draw command.

#include <vector>
class TH1;
class TH2;

struct DrawResult {
  int status = 0;
  TH2* hdraw = 0;
  TH1* hdrawx = 0;
  TH1* hdrawy = 0;
  std::vector<TH1*> hdrawxChan;
  TH1* ticks() const;
  TH1* channels() const;
  TH1* ticksForChannel(unsigned int chan) const;
};
