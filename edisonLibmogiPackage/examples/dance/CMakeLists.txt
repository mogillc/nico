cmake_minimum_required (VERSION 2.6)

project (dance)
set(CMAKE_BUILD_TYPE Debug)

# The version number
set (dance_VERSION_MAJOR 0)
set (dance_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../libmogi/cmake/Modules/")

#add_executable(dance src/dance2.cpp)
add_executable(danceNicoCurses	src/danceNicoCurses.cpp
src/CursesHandler.h
src/CursesHandler.cpp
src/danceState.h
src/danceState.cpp)

add_executable(danceDmitriCurses	src/danceDmitriCurses.cpp
src/CursesHandler.h
src/CursesHandler.cpp
src/danceState.h
src/danceState.cpp)

add_executable(danceNicoOsc	src/danceNicoOsc.cpp
src/CursesHandler.h
src/CursesHandler.cpp
src/danceState.h
src/danceState.cpp)




add_executable (Dancing src/Dancing.cpp src/danceState.h src/danceState.cpp src/Lighting.h src/Lighting.cpp src/danceParameters.h src/danceParameters.cpp)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})


IF(WIN32)
set(IpSystemTypePath ip/win32)
set(LIBS ${LIBS} Ws2_32 winmm)
ELSE(WIN32)
set(IpSystemTypePath ip/posix)
ENDIF(WIN32)

ADD_LIBRARY(oscpack

ip/IpEndpointName.h
ip/IpEndpointName.cpp

ip/NetworkingUtils.h
${IpSystemTypePath}/NetworkingUtils.cpp

ip/UdpSocket.h
${IpSystemTypePath}/UdpSocket.cpp

ip/PacketListener.h
ip/TimerListener.h

osc/OscTypes.h
osc/OscTypes.cpp
osc/OscHostEndianness.h
osc/OscException.h
osc/OscPacketListener.h
osc/MessageMappingOscPacketListener.h
osc/OscReceivedElements.h
osc/OscReceivedElements.cpp
osc/OscPrintReceivedElements.h
osc/OscPrintReceivedElements.cpp
osc/OscOutboundPacketStream.h
osc/OscOutboundPacketStream.cpp

)

ADD_EXECUTABLE(SimpleReceive src/SimpleReceive.cpp)
TARGET_LINK_LIBRARIES(SimpleReceive oscpack ${LIBS})




#	Find Mogi:
find_package (LibMogi REQUIRED)
include_directories(${LIBMOGI_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBMOGI_Libraries})


#	Find Sixaxis:
#find_package (LibSixaxis REQUIRED)
#include_directories(${LIBSIXAXIS_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${LIBSIXAXIS_Libraries})

#	include MIDI:
#find_package (LibPortmidi REQUIRED)
#include_directories(${LIBPORTMIDI_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${LIBPORTMIDI_Libraries})


#target_link_libraries (dance ${LIBMOGI_LIBRARIES})
#target_link_libraries (dance ${LIBSIXAXIS_LIBRARIES})
##target_link_libraries (dance ${LIBPORTMIDI_LIBRARIES})


target_link_libraries (Dancing ncurses)
target_link_libraries (Dancing ${LIBMOGI_LIBRARIES})
target_link_libraries (Dancing oscpack)
target_link_libraries (Dancing -lpthread)

target_link_libraries (danceNicoCurses ${LIBMOGI_LIBRARIES})
target_link_libraries (danceNicoCurses ${LIBSIXAXIS_LIBRARIES})
target_link_libraries (danceNicoCurses ncurses)

target_link_libraries (danceDmitriCurses ${LIBMOGI_LIBRARIES})
target_link_libraries (danceDmitriCurses ${LIBSIXAXIS_LIBRARIES})
target_link_libraries (danceDmitriCurses ncurses)


target_link_libraries (danceNicoOsc ${LIBMOGI_LIBRARIES})
target_link_libraries (danceNicoOsc ${LIBSIXAXIS_LIBRARIES})
target_link_libraries (danceNicoOsc ncurses)
target_link_libraries (danceNicoOsc oscpack)
target_link_libraries (danceNicoOsc -lpthread)


# Install
if(${UNIX})

set_target_properties(danceNicoCurses    PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install( 	TARGETS danceNicoCurses
DESTINATION bin
)

set_target_properties(danceDmitriCurses    PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install( 	TARGETS danceDmitriCurses
DESTINATION bin
)

set_target_properties(danceNicoOsc    PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install( 	TARGETS danceNicoOsc
DESTINATION bin
)

set_target_properties(Dancing    PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install( 	TARGETS Dancing
DESTINATION bin
)
endif(${UNIX})
