// getLabel.h
//
// David Adams
// October 2015
//
// Function to define a plot label from a text file.
//  fname = name of the file
//  draw = Draw if true.
// File format:
//   Title
//   xlab ylab
//

#include "TLatex.h"
#include <string>

TLatex* getLabel(std::string fname, bool draw =false, int dbg =0);
