cmake_minimum_required(VERSION 2.6)

PROJECT(mwerAlign)

#if you don't want the full compiler output, remove the following line
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ")
SET(CMAKE_VERBOSE_MAKEFILE ON)
#SET( CMAKE_CXX_FLAGS " -Wno-deprecated " CACHE STRING "Flags used by the C++ compiler" FORCE )
#add_definitions(-DCMAKE_INSTALL_PREFIX=YOUR INSTALL DIR HERE!!!)
add_definitions(-DIBPP_LINUX -DIBPP_GCC)
# include_directories(YOUR INCLUDE DIR HERE!!!)
# link_directories(YOUR LIB DIR HERE!!!)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2 -static)

#list all source files here
add_subdirectory(src)
add_subdirectory(test)

#ADD_EXECUTABLE(tercpp main.cpp)

#install(TARGETS tercpp DESTINATION bin)

#need to link to some other libraries ? just add them here
#TARGET_LINK_LIBRARIES(tercpp png jpeg)
 
