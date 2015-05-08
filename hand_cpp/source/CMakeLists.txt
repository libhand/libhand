CMAKE_MINIMUM_REQUIRED (VERSION 2.8.6)

IF(APPLE)
  LIST(APPEND CMAKE_PREFIX_PATH "/opt/local")
  LIST(APPEND CMAKE_MODULE_PATH "/opt/local/share/cmake/modules")
  LIST(APPEND CMAKE_FRAMEWORK_PATH "/opt/local/Library/Frameworks")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.framework;.dylib;.so")
  LIST(APPEND LibHand_EXTRA_LIBS "-lboost_system -lboost_date_time -lz -lm -lbz2")
  SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
ELSEIF(UNIX)
    LIST(APPEND CMAKE_MODULE_PATH "/usr/share/OGRE/cmake/modules")
    LIST(APPEND LibHand_EXTRA_LIBS "-ldl -lXt -lboost_filesystem -lboost_system")
    ADD_DEFINITIONS(-fPIC)
ENDIF()

IF(UNIX OR APPLE)
  ADD_DEFINITIONS(-O3)
ENDIF()

IF(WIN32)
  # Remember to always use forward slashes in CMake, even for Windows paths,
  # as backslash followed by a character is interpreted as escape sequences.
  # We use both locations Chocolatey package manager installed into, and
  # relative paths for the dependencies that need to be compiled from source.

  SET(BOOST_LOCALLY_INSTALLED_DIRECTORY "${PROJECT_SOURCE_DIR}/third_party/boost_1_58_0/")
  IF(EXISTS ${BOOST_LOCALLY_INSTALLED_DIRECTORY}) 
    MESSAGE(STATUS "Using local Boost directory as hints for FIND_PACKAGE")
    SET(BOOST_INCLUDEDIR "${BOOST_LOCALLY_INSTALLED_DIRECTORY}")
    SET(BOOST_LIBRARYDIR "${BOOST_LOCALLY_INSTALLED_DIRECTORY}/stage/win32/lib")
  ELSE()
    MESSAGE(STATUS "Using Chocolatey's Boost install directory as hints for FIND_PACKAGE")
    SET(BOOST_INCLUDEDIR "C:/local/boost_1_58_0")
    SET(BOOST_LIBRARYDIR "C:/local/boost_1_58_0/win32/lib")
  ENDIF()

  set(Boost_USE_STATIC_LIBS ON)
  FIND_PACKAGE(Boost COMPONENTS filesystem thread system REQUIRED)
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

  # Debug variables populated from FIND_PACKAGE
  message (status "** Boost include ${Boost_INCLUDE_DIR}")
  message (status "** Boost libdirs ${Boost_LIBRARY_DIRS}")
  message (status "** Boost libraries ${Boost_LIBRARIES}")

  # If you get a link error relating to Boost filesystem and 'convert',
  # It may be due to the wchar_t vs wchar_t- flag
  # ADD_DEFINITIONS(/Zc:wchar_t-) 

  SET(OpenCV_DIR "C:/OpenCV249/opencv/build")
 
  # If a local OGRE path exists, use that rather than system install
  SET(OGRE_LOCALLY_INSTALLED_DIRECTORY "${PROJECT_SOURCE_DIR}/third_party/OgreSDK_vc11_v1-9-0")
  IF(EXISTS "${OGRE_LOCALLY_INSTALLED_DIRECTORY}")
    MESSAGE(STATUS "Using local OGRE install in CMake lookup path")
    LIST(APPEND CMAKE_MODULE_PATH "${OGRE_LOCALLY_INSTALLED_DIRECTORY}/CMake")
    # Even with local OGRE install, OGRE_HOME env variable is required! 
    SET(ENV{OGRE_HOME} "${OGRE_LOCALLY_INSTALLED_DIRECTORY}")
  ELSE()
    MESSAGE(STATUS "Using system OGRE install in CMake lookup path: $ENV{OGRE_HOME}")
    # Appending to the module path worked, but setting OGRE_DIR to $ENV{OGRE_HOME} didn't
    # The OGRE_HOME variable set by the OGRE installer contains a "\O" which kills CMake
    # We want to use the regex \\  which codes for backslash. With CMake intepreting, we
    # need four backslashes.
    STRING(REGEX REPLACE "\\\\" "/" OGRE_HOME_WITH_FORWARD_SLASHES $ENV{OGRE_HOME})
    LIST(APPEND CMAKE_MODULE_PATH "${OGRE_HOME_WITH_FORWARD_SLASHES}/CMake")
  ENDIF() 
ENDIF()


# Uncomment the following line for development of the library
# It will help against unwanted bugs
# INCLUDE(copts_stricter_warnings.cmake)
MESSAGE(STATUS "CMAKE_MODULE_PATHS is ${CMAKE_MODULE_PATH}")

SET(BUILD_SHARED_LIBS ON)

FIND_PACKAGE(OpenCV REQUIRED)

SET(OGRE_STATIC FALSE)
IF (OGRE_STATIC)
  # OGRE has different plugin loading API calls depending on how it was linked
  ADD_DEFINITIONS(LOAD_OGRE_PLUGINS_STATICALLY)
ENDIF()

FIND_PACKAGE(OGRE REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost_FOUND is true!")
endif()
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(
  ${OGRE_INCLUDE_DIRS}
  ${OGRE_RenderSystem_GL_INCLUDE_DIR}
  ${OGRE_Plugin_BSPSceneManager_INCLUDE_DIR}
  ${OGRE_Plugin_OctreeSceneManager_INCLUDE_DIR}
  ${OGRE_Plugin_PCZSceneManager_INCLUDE_DIR}
  ${OGRE_Plugin_ParticleFX_INCLUDE_DIR}
)

ADD_SUBDIRECTORY(dot_sceneloader)
INCLUDE_DIRECTORIES(dot_sceneloader)

SET(MACSRCS "")

IF(APPLE)
  LIST(APPEND MACSRCS "mac_app_loop.mm")
ENDIF()

ADD_LIBRARY(hand_utils
  ${MACSRCS}
  error_handling.cc
  file_dialog.cc
  file_tools.cc
  image_utils.cc
  printfstring.cc
  simple_slider.cc
  text_printer.cc)


# Visual Studio 2013 only adds a subset of boost libraries to linker path if we don't explicitly link
TARGET_LINK_LIBRARIES(hand_utils
  ${OpenCV_LIBS}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_LIBRARIES}
  ${LibHand_EXTRA_LIBS}
)

ADD_LIBRARY(hand_hog
  hog_cell.cc
  hog_descriptor.cc
  hog_cell_rectangles.cc
  image_to_hog_calculator.cc
  hog_utils.cc)

TARGET_LINK_LIBRARIES(hand_hog
  hand_utils
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES})

ADD_LIBRARY(hand_renderer
  hand_renderer.cc
  hand_camera_spec.cc
  hand_pose.cc
  scene_spec.cc)

TARGET_LINK_LIBRARIES(hand_renderer
  dot_sceneloader
  hand_utils
  ${EXTRA_MAC_LIBS}
  ${EXTRA_LINUX_LIBS}
  ${OpenCV_LIBS}
  ${OGRE_LIBRARIES}
  ${OGRE_LIBRARY}
  ${OGRE_RenderSystem_GL_LIBRARIES}
  ${OGRE_Plugin_OctreeSceneManager_LIBRARIES})

message(STATUS "==================================================")
message(STATUS "   Build diagnostics:")
message(STATUS "")
message(STATUS "Boost: ${Boost_LIBRARIES}")
message(STATUS "OpenCV: ${OpenCV_LIBS}")
message(STATUS "OgreLib: ${OGRE_LIBRARY}")
message(STATUS "Ogre: ${OGRE_LIBRARIES}")
message(STATUS "Ogre GL: ${OGRE_RenderSystem_GL_LIBRARIES}")
message(STATUS "Ogre Octree: ${OGRE_Plugin_OctreeSceneManager_LIBRARIES}")
message(STATUS "X11_FOUND: ${X11_FOUND}")
message(STATUS "XAW_LIBRARY: ${XAW_LIBRARY}")
message(STATUS "OGRE_LIBRARY_FWK: ${OGRE_LIBRARY_FWK}")
message(STATUS "ZZip_LIBRARIES: ${ZZip_LIBRARIES}")
message(STATUS "ZLIB_LIBRARIES: ${ZLIB_LIBRARIES}")
message(STATUS "FreeImage_LIBRARIES: ${FreeImage_LIBRARIES}")
message(STATUS "FREETYPE_LIBRARIES: ${FREETYPE_LIBRARIES}")
message(STATUS "X11_LIBRARIES: ${X11_LIBRARIES}")
message(STATUS "X11_Xrandr_LIB: ${X11_Xrandr_LIB}")
message(STATUS "Cocoa_LIBRARIES: ${Cocoa_LIBRARIES}")
message(STATUS "Carbon_LIBRARIES: ${Carbon_LIBRARIES}")
message(STATUS "==================================================")
install(TARGETS
  hand_hog
  hand_renderer
  hand_utils
  EXPORT LibHand-export DESTINATION ${HAND_LIB_DIST_DIR})

install(EXPORT LibHand-export DESTINATION ${HAND_LIB_DIST_DIR})

ADD_EXECUTABLE(pose_designer
  pose_designer_main.cc
  pose_designer.cc)

TARGET_LINK_LIBRARIES(pose_designer
  hand_hog
  hand_renderer
  hand_utils
  ${Boost_LIBRARIES})

SET(LibHand_INCLUDE_DIRS
  ${CMAKE_CURRENT_LIST_DIR}
  ${Boost_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${OGRE_INCLUDE_DIRS}
  ${OGRE_RenderSystem_GL_INCLUDE_DIR}
  ${OGRE_Plugin_BSPSceneManager_INCLUDE_DIR}
  ${OGRE_Plugin_OctreeSceneManager_INCLUDE_DIR}
  ${OGRE_Plugin_PCZSceneManager_INCLUDE_DIR}
  ${OGRE_Plugin_ParticleFX_INCLUDE_DIR}
)

CONFIGURE_FILE(LibHandConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LibHandConfig.cmake @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/LibHandConfig.cmake
  DESTINATION ${HAND_LIB_DIST_DIR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/LibHandConfig.cmake
  DESTINATION ${HAND_LIB_DIST_DIR}
  RENAME FindLibHand.cmake)
