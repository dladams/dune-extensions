dune_extensions/run

This directory provides scripts and parameter fiels for producing and
analyzing event samples.

Scripts to run jobs
* prod: Script to run a production job.
* perf: Script to run a performance job.

Parameter files
* genrec35t.fcl:   Gen + reco for 35t
* genrec10kt.fcl:  Gen + reco for 10kT far detector
* genrec10ktw.fcl: Gen + reco for workspace detector (subset of 10 kt)
* deconvolute.fcl: Deconvolution
* hitfinder.fcl:   Hit finding
* perf35t.fcl:     Generate performance results for 35t
* perf10kt.fcl:    Generate performance results for 10kt
* perf10ktw.fcl:   Generate performance results for 10kt workspace

Other
* valgrind_suppressions.txt: Suppresions for valgrind
