simple_plugin(DXDisplay "module"
    DXPerf
    DXGeometry
    DXUtil
    dune_DuneCommon
    ${Boost_FILESYSTEM_LIBRARY}
)

install_fhicl()
install_source( README.txt )
