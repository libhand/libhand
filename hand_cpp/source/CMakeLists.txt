CMAKE_MINIMUM_REQUIRED (VERSION 2.8.6)

IF(APPLE)
  LIST(APPEND CMAKE_PREFIX_PATH "/opt/local")
  LIST(APPEND CMAKE_MODULE_PATH "/opt/local/share/cmake/modules")
  LIST(APPEND CMAKE_FRAMEWORK_PATH "/opt/local/Library/Frameworks")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.framework;.dylib;.so")
ELSE(APPLE)
  IF(UNIX)
    LIST(APPEND CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake")
    LIST(APPEND LibHand_EXTRA_LIBS "-ldl -lXt")
    ADD_DEFINITIONS(-fPIC)
  ENDIF()
ENDIF(APPLE)

ADD_DEFINITIONS(-O3)

# Uncomment the following line for development of the library
# It will help against unwanted bugs
# INCLUDE(copts_stricter_warnings.cmake)

SET(BUILD_SHARED_LIBS OFF)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)

SET(OGRE_STATIC TRUE)
FIND_PACKAGE(OGRE REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

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

TARGET_LINK_LIBRARIES(hand_utils ${OpenCV_LIBS})

ADD_LIBRARY(hand_hog
  hog_cell.cc
  hog_descriptor.cc
  hog_cell_rectangles.cc
  image_to_hog_calculator.cc
  hog_utils.cc)

TARGET_LINK_LIBRARIES(hand_hog
  hand_utils
  ${OpenCV_LIBS})

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
  ${OGRE_RenderSystem_GL_LIBRARIES}
  ${OGRE_LIBRARY}
  ${OGRE_Plugin_OctreeSceneManager_LIBRARIES})

message(STATUS "==================================================")
message(STATUS "   Build diagnostics:")
message(STATUS "")
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
  hand_utils)

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
