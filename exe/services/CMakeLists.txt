cet_make_exec(channel_map
  SOURCE channel_map.cxx
  LIBRARIES
    dune_ArtSupport
    larcore_Geometry
    cetlib
    larcore_Geometry
    ${ROOT_BASIC_LIB_LIST}
)

install_source()
