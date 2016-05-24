cet_test(test_TpcSignalMap SOURCES test_TpcSignalMap.cxx
  LIBRARIES DXPerf dune_Geometry
)

cet_test(test_TpcSignalMapComparison SOURCES test_TpcSignalMapComparison.cxx
  LIBRARIES DXPerf dune_Geometry
)

cet_test(test_TpcSignalMatcher SOURCES test_TpcSignalMatcher.cxx
  LIBRARIES DXPerf dune_Geometry
)

cet_test(test_TpcSignalMatchTree SOURCES test_TpcSignalMatchTree.cxx
  LIBRARIES DXPerf dune_ArtSupport dune_Geometry
)

cet_test(test_MCTrajectoryFollower SOURCES test_MCTrajectoryFollower.cxx
  LIBRARIES DXPerf dune_ArtSupport
)

cet_test(test_SimChannelTupler SOURCES test_SimChannelTupler.cxx
  LIBRARIES DXPerf dune_ArtSupport larsim_Simulation
)
