# dune_extensions/DXGeometry/CMakeLists.txt
#
# David Adams
# May 2016

#include(CetRootCint)
#include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
#cet_rootcint( "DXGeometry" )

#set( PACKAGE DXGeometry )
#FILE( GLOB src_files *.cxx )
#cet_rootcint( ${PACKAGE} )

#cet_rootcint( "DXGeometry_dict" )


#MESSAGE( STATUS "DXGeometry PACKAGE: " ${PACKAGE} )
#MESSAGE( STATUS "DXGeometry src_files: " ${src_files} )
MESSAGE( STATUS "DXGeometry libs: " ${CETLIB} ${CETLIB_EXCEPT} )

art_make(BASENAME_ONLY
         LIB_LIBRARIES
           dune_Geometry
           larcore_Geometry
           lardata_RawData_utils
           lardataobj_RecoBase
           fhiclcpp
           ${SIMULATIONBASE}
           ${ART_FRAMEWORK_CORE}
           ${ART_FRAMEWORK_PRINCIPAL}
           ${ART_FRAMEWORK_SERVICES_REGISTRY}
           ${ART_FRAMEWORK_SERVICES_OPTIONAL}
           ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
           ${ART_PERSISTENCY_COMMON}
           ${ART_PERSISTENCY_PROVENANCE}
           ${ART_UTILITIES}
           ${CANVAS}
           ${MF_MESSAGELOGGER}
           ${MF_UTILITIES}
           ${CETLIB}
           ${CETLIB_EXCEPT}
           ${ROOT_EVE_LIB_LIST}
           ${ROOT_BASIC_LIB_LIST}
           ${Boost_SYSTEM_LIBRARY}
         MODULE_LIBRARIES
           DXGeometry
)

install_headers()
install_fhicl()
install_source( README.txt )
install_scripts()


