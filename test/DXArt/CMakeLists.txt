cet_test(test_TFileService SOURCES test_TFileService.cxx
  LIBRARIES
    art_Framework_Services_Optional_TFileService_service
    art_Framework_Services_System_TriggerNamesService_service
    art_Framework_EventProcessor
)

install_headers()
install_source()
