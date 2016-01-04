// dxprint.h
//
// David Adams
// January 2016
//
// Function to print the current canvas.

#ifndef DXPRINT_H
#define DXPRINT_H

#include <string>
#include "TLatex.h"

// Save the current canvas in file fname.fext.
// If fname or fext is blank, then the last value is used it its place.
// act specifies the action to take:
//   0 - Do not save canvas
//   1 - Save canvas as is
//   2 - Add label with dxlabel() and then save canvas
// Returns 0 for success.
int dxprint(std::string fname ="", std::string fext ="", int act =0);

#endif
