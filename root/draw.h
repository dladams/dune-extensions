// draw.h
//
// David Adams
// October 2015
//
// Root function to draw the channel-tick histograms produced
// by the dune_extensions package.

#include <string>
#include "DrawResult.h"

class TCanvas;

// This is the canvas where the 2D plot is rendered.
extern TCanvas* drawCanvas;

DrawResult draw(std::string name ="help", int how =0, double zmax =0.0,
                double xmin =0.0, double xmax =0.0, double ymin =0.0, double ymax =0.0);
