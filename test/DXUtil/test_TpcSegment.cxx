// test_TpcSegment.cxx

// David Adams
// May 2015
//
// Test script for Range.

#include "DXUtil/TpcSegment.h"

#include <string>
#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;

int main() {
  const string myname = "test_TpcSegment: ";
  cout << myname << "Starting test" << endl;
#ifdef NDEBUG
  cout << myname << "NDEBUG must be off." << endl;
  abort();
#endif
  TpcSegment seg(11, 1.0, 2.0, 3.0, 10.0, 1);
  cout << "Segment length: " << seg.length << endl;
  assert(seg.length == 0.0);
  seg.addPoint(2.0, 3.0, 4.0, 9.0);
  cout << "Segment length: " << seg.length << endl;
  seg.addPoint(3.0, 4.0, 5.0, 8.0);
  cout << "Segment length: " << seg.length << endl;
  assert(seg.tpc = 11);
  assert(seg.x1 = 1.0);
  assert(seg.y1 = 1.0);
  assert(seg.z1 = 1.0);
  assert(seg.x2 = 3.0);
  assert(seg.y2 = 4.0);
  assert(seg.z2 = 5.0);
  assert(seg.e1 = 10.0);
  assert(seg.e2 =  8.0);
  cout << "Segment length: " << seg.length << endl;
  assert(seg.length > 3.46);
  assert(seg.length < 3.47);
  cout << myname << "Ending test" << endl;
  return 0;
}
