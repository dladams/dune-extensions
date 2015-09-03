cet_test(test_Range SOURCES test_Range.cxx
)

cet_test(test_intProcess SOURCES test_intProcess.cxx
  LIBRARIES DXUtil
)

cet_test(test_reducedPDG SOURCES test_reducedPDG.cxx
  LIBRARIES DXUtil
)

