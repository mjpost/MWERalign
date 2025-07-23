PROJECT(mwerAlign)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

SET(mwerAlign_BIN_SRCS
   ${CMAKE_SOURCE_DIR}/src/main.cc
   ${CMAKE_SOURCE_DIR}/src/SimpleText2.cc
   ${CMAKE_SOURCE_DIR}/src/Evaluator.cc
   ${CMAKE_SOURCE_DIR}/src/gzstream.cc
   ${CMAKE_SOURCE_DIR}/src/IOfile.cc
   ${CMAKE_SOURCE_DIR}/src/mwerAlign.cc
)

add_executable(mwerAlign ${mwerAlign_BIN_SRCS})
target_link_libraries(mwerAlign z mwerAlignLib)

install(TARGETS mwerAlign DESTINATION bin)

