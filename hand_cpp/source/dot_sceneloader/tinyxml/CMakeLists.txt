ADD_LIBRARY(tinyxml STATIC
  tinyxml.cpp tinyxmlparser.cpp xmltest.cpp tinyxmlerror.cpp tinystr.cpp)

install(TARGETS tinyxml EXPORT LibHand-export DESTINATION ${HAND_LIB_DIST_DIR})
