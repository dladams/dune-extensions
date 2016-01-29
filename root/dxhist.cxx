// dxhist.cxx

#include "dxhist.h"

namespace {
  std::string dxname;
}

void dxhist(std::string name) {
  dxname = name;
}

std::string dxhist() {
  return dxname;
}
