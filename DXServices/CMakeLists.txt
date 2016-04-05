# dune_extensions/DXServices/CMakeLists.txt
#
# David Adams
# February 2016
#
# CMake instructions for DX services

# Add plugin for each service.

simple_plugin(DXRawDisplayService "service"
  DXUtil
  DXGeometry
             )

install_fhicl()
#add_subdirectory(test)
