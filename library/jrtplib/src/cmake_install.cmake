# Install script for directory: /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/jrtplib3" TYPE FILE FILES
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpapppacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpbyepacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpcompoundpacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpcompoundpacketbuilder.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcppacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcppacketbuilder.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcprrpacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpscheduler.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpsdesinfo.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpsdespacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpsrpacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtcpunknownpacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpaddress.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpcollisionlist.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpconfig.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpdebug.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpdefines.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtperrors.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtphashtable.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpinternalsourcedata.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpipv4address.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpipv4destination.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpipv6address.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpipv6destination.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpkeyhashtable.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtplibraryversion.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpmemorymanager.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpmemoryobject.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtppacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtppacketbuilder.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtppollthread.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtprandom.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtprandomrand48.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtprandomrands.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtprandomurandom.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtprawpacket.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsession.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsessionparams.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsessionsources.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsourcedata.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsources.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpstructs.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtptimeutilities.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtptransmitter.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtptypes_win.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtptypes.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpudpv4transmitter.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpudpv6transmitter.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpbyteaddress.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpexternaltransmitter.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsecuresession.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpsocketutil.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpabortdescriptors.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtpselect.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtptcpaddress.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/rtptcptransmitter.h"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/extratransmitters/rtpfaketransmitter.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libjrtp.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/libjrtp.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libjrtp.so.3.11.1;/usr/local/lib/libjrtp.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/libjrtp.so.3.11.1"
    "/snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/src/libjrtp.so"
    )
endif()

