# dune_extensions/test/chanmap/CMakeLists.txt
#
# David Adams
# April 2016
#
# Instructions to build and install chanmap.

cet_make_exec(chanmap
  SOURCE chanmap.cxx
  LIBRARIES
    larcore_Geometry
    AXService
    cetlib
    ${ROOT_BASIC_LIB_LIST}
)

install_source()
