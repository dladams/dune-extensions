# dune_extensions/root/CMakeLists.txt
#
# David Adams
# October 2015
#
# install Root scripts in rootscripts directory in the build and product areas.

FILE(GLOB ROOTSRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h *.C *.cxx)
#LIST(REMOVE_ITEM ROOTSRCS "rootlogon.C")
message("Root sources: " ${ROOTSRCS})
cet_install_files(DIRNAME rootscripts LIST ${ROOTSRCS})

install_headers()
