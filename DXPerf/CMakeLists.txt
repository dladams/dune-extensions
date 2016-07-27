# DXPerf/CMakeLists.txt
#
# David Adams
# September 2015
#
# CMake instructions for the DXPerf library which holds
# classes for evaluating DUNE reco performance.

art_make(BASENAME_ONLY
         LIB_LIBRARIES
           DXUtil
           DXGeometry
           canvas_Persistency_Common
           canvas_Persistency_Provenance
           nusimdata_SimulationBase
         MODULE_LIBRARIES
           DXPerf
)

install_headers()
install_fhicl()
install_source( README.txt )
install_scripts()
