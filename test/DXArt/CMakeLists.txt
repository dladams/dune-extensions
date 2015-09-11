cet_test(test_TFileService SOURCES test_TFileService.cxx
  LIBRARIES
    art_Framework_Services_Optional_TFileService_service
    art_Framework_Services_System_TriggerNamesService_service
    art_Framework_EventProcessor
)

cet_test(test_ArtServiceHelper SOURCES test_ArtServiceHelper.cxx
  LIBRARIES
    DXArt
    art_Framework_Services_Optional_TFileService_service
    art_Framework_Services_Optional_RandomNumberGenerator_service
)

install_headers()
install_source()
