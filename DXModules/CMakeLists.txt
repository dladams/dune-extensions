
art_make(BASENAME_ONLY
         MODULE_LIBRARIES
           DXPerf
         LIB_LIBRARIES
           DXPerf
           DXGeometry
           DXUtil
           ${Boost_FILESYSTEM_LIBRARY}
)

install_fhicl()
install_source( README.txt )
