CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

FIND_PACKAGE(LibHand REQUIRED
  PATHS "../hand_cpp/dist"
  NO_MODULE
  NO_DEFAULT_PATH)

include_directories(${LibHand_INCLUDE_DIRS})

ADD_EXECUTABLE(bend_one_finger bend_one_finger.cc)
TARGET_LINK_LIBRARIES(bend_one_finger ${LibHand_LIBRARIES})

ADD_EXECUTABLE(render_hog_descriptor render_hog_descriptor.cc)
TARGET_LINK_LIBRARIES(render_hog_descriptor ${LibHand_LIBRARIES})

ADD_EXECUTABLE(file_dialog_test file_dialog_test.cc)
TARGET_LINK_LIBRARIES(file_dialog_test hand_utils)
