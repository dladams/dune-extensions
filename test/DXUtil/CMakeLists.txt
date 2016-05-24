cet_test(test_Range SOURCES test_Range.cxx
)

cet_test(test_intProcess SOURCES test_intProcess.cxx
  LIBRARIES DXUtil
)

cet_test(test_reducedPDG SOURCES test_reducedPDG.cxx
  LIBRARIES DXUtil
)

cet_test(test_TpcTypes SOURCES test_TpcTypes.cxx
  LIBRARIES DXUtil
)

cet_test(test_TpcSegment SOURCES test_TpcSegment.cxx
  LIBRARIES DXUtil
)

cet_test(test_ChannelTickHistCreator SOURCES test_ChannelTickHistCreator.cxx
  LIBRARIES DXUtil dune_ArtSupport ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
)

