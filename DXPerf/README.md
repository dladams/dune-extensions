# dune_extensions/DXPerf

Extensions to DUNE SW for assessing performance of simulation and reconstruction.

* TpcSignalMap: Class that holds signals indexed by TPC channel and tick.
* TpcSignalMapComparison: Class to compare two TpcSignalMap objects.
* TpcSignalMatcher: Class to pair the objects in two TpcSignalMap vectors.
* TpcSignalMatchTree: Class to build a Root tree from a TpcSignalMatcher.
* SimChannelTupler: Class to build a Root tree from a vector of SimChannel objects.
* MCTrajectoryFollower: Class follow MCParticle trajectories and fill a Root tree and TpcSignalMap objects.
