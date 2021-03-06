cmake_minimum_required(VERSION 2.8.3)
project(spatial_ins)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS 
    roscpp 
    snippets 
    geometry_msgs
    sensor_msgs
    tf2_ros)

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system thread regex serialization) 

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES navquest_dvl
#  CATKIN_DEPENDS auv_msgs roscpp snippets
#  DEPENDS system_lib
)

###########
## Build ##
###########
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  src/test
  ${Boost_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp library
# add_library(navquest_dvl
#   src/${PROJECT_NAME}/navquest_dvl.cpp
# )

## Declare a cpp executable
add_executable(spatial_node 
	src/SpatialNode.cpp)
target_link_libraries(spatial_node ${Boost_THREAD_LIBRARY} ${Boost_SERIALIZATION_LIBRARY} ${catkin_LIBRARIES})
#add_executable(rtcm_repacker src/test/rtcm_repacker.cpp)
#add_executable(rtcm2.3_repacker src/test/rtcm2.3_repacker.cpp src/test/rtcm.c src/test/rtcm2.c src/test/rtcm3.c src/test/rtcm3e.c src/test/rtkcmn.c)
#add_executable(rtcm2.3_repacker_clean src/test/rtcm2.3_repacker_clean.cpp)
#add_executable(rtcm2.3_repacker_timed src/test/rtcm2.3_repacker_timed.cpp)
#add_executable(rtcm2.3_repacker_unclean src/test/rtcm2.3_repacker_unclean.cpp src/test/rtcm.c src/test/rtcm2.c src/test/rtcm3.c src/test/rtcm3e.c src/test/rtkcmn.c)
#add_executable(bitptr_tester src/test/bitptr_test.cpp src/test/rtcm.c src/test/rtcm2.c src/test/rtcm3.c src/test/rtcm3e.c src/test/rtkcmn.c)
##add_executable(serial_sink src/test/serial_sink.cpp)
#	
#add_executable(spatial_test src/test/message_test.cpp)
#
#target_link_libraries(rtcm_repacker
#   ${Boost_THREAD_LIBRARY}
# # ${Boost_REGEX_LIBRARY}
#   ${Boost_SERIALIZATION_LIBRARY}
#   ${catkin_LIBRARIES}
#)
#
#target_link_libraries(rtcm2.3_repacker
#   ${Boost_THREAD_LIBRARY}
# # ${Boost_REGEX_LIBRARY}
#   ${Boost_SERIALIZATION_LIBRARY}
#   ${catkin_LIBRARIES}
#   rt
#)
#
#target_link_libraries(rtcm2.3_repacker_clean
#   ${Boost_THREAD_LIBRARY}
# # ${Boost_REGEX_LIBRARY}
#   ${Boost_SERIALIZATION_LIBRARY}
#   ${catkin_LIBRARIES}
#)
#
#target_link_libraries(rtcm2.3_repacker_timed
#   ${Boost_THREAD_LIBRARY}
# # ${Boost_REGEX_LIBRARY}
#   ${Boost_SERIALIZATION_LIBRARY}
#   ${catkin_LIBRARIES}
#)
#
#target_link_libraries(rtcm2.3_repacker_unclean
#   ${Boost_THREAD_LIBRARY}
# # ${Boost_REGEX_LIBRARY}
#   ${Boost_SERIALIZATION_LIBRARY}
#   ${catkin_LIBRARIES}
#   rt
#)
#
#target_link_libraries(serial_sink
#   ${Boost_SERIALIZATION_LIBRARY}
#   ${catkin_LIBRARIES}
#)
#
#target_link_libraries(bitptr_tester rt)
#
#target_link_libraries(spatial_test ${catkin_LIBRARIES}
#)

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS navquest_dvl navquest_dvl_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_navquest_dvl.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
