// dxlabel.h
//
// David Adams
// January 2016
//
// Function that holds a string label and returns Root
// text objects with that name.

#ifndef DXLABEL_H
#define DXLABEL_H

#include <string>
#include "TLatex.h"

// Return  new text object.
// Provide non-blank argument to reset the name.
TLatex* dxlabel(std::string lab ="");

#endif
