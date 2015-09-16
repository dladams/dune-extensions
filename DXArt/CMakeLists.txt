# DXArt/CMakeLists.txt
#
# David Adams
# September 2015
#
# CMake instructions for the DXArt library which holds
# helper classes for art.

art_make(BASENAME_ONLY
  LIB_LIBRARIES
    art_Framework_EventProcessor
    art_Framework_Services_Registry
    art_Utilities
    fhiclcpp
    cetlib
    ${Boost_SYSTEM_LIBRARY}
    art_Framework_Services_System_TriggerNamesService_service
    art_Framework_Services_System_CurrentModule_service
)

install_headers()
install_fhicl()
install_source( README.txt )
install_scripts()
