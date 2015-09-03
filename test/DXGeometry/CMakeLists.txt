#cet_test(test_Range SOURCES test_Range.cxx
  #LIBRARIES LbTupler
#)

cet_test(test_geometry SOURCES test_geometry.cxx
  LIBRARIES dune_Geometry DXGeometry
)

cet_test(draw_geometry SOURCES draw_geometry.cxx
  LIBRARIES DXGeometry
)

install_headers()
install_source()

