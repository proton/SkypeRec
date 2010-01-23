
# versions before 2.4.8 do not seem to know about Qt's DBus
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.8 FATAL_ERROR)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(skyperec)
SET(TARGET skyperec)

SET(CMAKE_BUILD_TYPE RELEASE)
#SET(CMAKE_BUILD_TYPE DEBUG)

# compiler flags

SET(CMAKE_CXX_FLAGS "-Wall -W -pipe -fno-exceptions")
SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -s")

# sources

SET(SOURCES
	src/call.cpp
	src/common.cpp
	src/gui.cpp
	src/mp3writer.cpp
	src/preferences.cpp
	src/recorder.cpp
	src/skype.cpp
	src/trayicon.cpp
	src/utils.cpp
	src/version.cpp
	src/vorbiswriter.cpp
	src/wavewriter.cpp
	src/writer.cpp
)

SET(MOC_HEADERS
	src/call.h
	src/gui.h
	src/preferences.h
	src/recorder.h
	src/skype.h
	src/smartwidgets.h
	src/trayicon.h
)

SET(RESOURCES resources.qrc)

# generation of version.cpp

ADD_CUSTOM_TARGET(Version
	${CMAKE_CURRENT_SOURCE_DIR}/makeversion ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Checking version.cpp" VERBATIM)
SET_SOURCE_FILES_PROPERTIES(version.cpp PROPERTIES GENERATED TRUE)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES src/version.cpp)
# required when building out of source, so version.cpp finds headers
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# external libraries

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
SET(LIBRARIES)

# lame

FIND_PACKAGE(lame REQUIRED)
INCLUDE_DIRECTORIES(${LAME_INCLUDE_DIR})
SET(LIBRARIES ${LIBRARIES} ${LAME_LIBRARY})

# id3

FIND_PACKAGE(id3 REQUIRED)
INCLUDE_DIRECTORIES(${ID3_INCLUDE_DIR})
SET(LIBRARIES ${LIBRARIES} ${ID3_LIBRARY})

# vorbisenc

FIND_PACKAGE(vorbisenc REQUIRED)
INCLUDE_DIRECTORIES(${VORBISENC_INCLUDE_DIR})
SET(LIBRARIES ${LIBRARIES} ${VORBISENC_LIBRARY})

# Qt

SET(QT_USE_QTDBUS TRUE)
SET(QT_USE_QTNETWORK TRUE)
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
SET(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
QT4_WRAP_CPP(MOC_SOURCES ${MOC_HEADERS})
QT4_ADD_RESOURCES(RESOURCES_SOURCES ${RESOURCES})
SET(SOURCES ${SOURCES} ${MOC_SOURCES} ${RESOURCES_SOURCES})

# main target

ADD_EXECUTABLE(${TARGET} ${SOURCES})
TARGET_LINK_LIBRARIES(${TARGET} ${LIBRARIES})
ADD_DEPENDENCIES(${TARGET} Version)

# installation

INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION bin)
INSTALL(FILES skyperec.desktop DESTINATION share/applications)
INSTALL(FILES icons/skyperec-128.png DESTINATION share/icons/hicolor/128x128/apps
	RENAME skyperec.png)

