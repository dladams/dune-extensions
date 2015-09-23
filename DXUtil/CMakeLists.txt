# DXUtil/CMakeLists.txt
#
# David Adams
# September 2015
#
# CMake isntructions for the DXUtil library which holds
# utility functions and classes.

art_make(BASENAME_ONLY
  LIB_LIBRARIES
    ${ART_FRAMEWORK_SERVICES_OPTIONAL}
    ${ROOT_BASIC_LIB_LIST}
    boost_system
)

install_headers()
install_fhicl()
install_source( README.txt )
install_scripts()
