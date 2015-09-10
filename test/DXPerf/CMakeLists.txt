cet_test(test_TpcSignalMap SOURCES test_TpcSignalMap.cxx
  LIBRARIES DXPerf dune_Geometry
)

cet_test(test_MCTrajectoryFollower SOURCES test_MCTrajectoryFollower.cxx
  LIBRARIES DXPerf
)
