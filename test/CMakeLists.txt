# ======================================================================
#
# Testing
#
# ======================================================================

include(CetTest)
cet_enable_asserts()

add_subdirectory( DXUtil )
add_subdirectory( DXGeometry )
add_subdirectory( DXPerf )
add_subdirectory( fcl )
