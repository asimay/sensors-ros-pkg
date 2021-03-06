cmake_minimum_required(VERSION 2.8.3)
project(usbl_comms)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
    roscpp
    nodelet
    auv_msgs
    snippets
    seatrac
    misc_msgs
    message_filters)

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system serialization thread)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

#######################################
## Declare ROS messages and services ##
#######################################

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs  # Or other packages containing msgs
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES usbl_comms
#  CATKIN_DEPENDS roscpp
#  DEPENDS system_lib
)

###########
## Build ##
###########
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(include
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp library
add_library(usbl_comms src/USBLManager.cpp src/DiverMsg.cpp)

add_library(uros_comms src/uros/uros_messages.cpp
    src/uros/uros_modem_controller.cpp
    src/uros/uros_usbl_controller.cpp)

add_library(caddy_comms src/caddy/caddy_messages.cpp
  src/caddy/buddy_usbl.cpp
  src/caddy/surface_usbl.cpp
  src/caddy/diver_modem.cpp
  src/caddy/nav_module.cpp
  src/caddy/buddy_handler.cpp
  src/caddy/surface_handler.cpp
  src/caddy/diver_handler.cpp)
#  src/caddy/command_module.cpp
#  src/caddy/status_handler.cpp)

## Declare a cpp executable
add_executable(divermsg_test src/test/divermsg_test.cpp src/DiverMsg.cpp)
add_executable(encoding_test src/test/bitencoding.cpp src/DiverMsg.cpp)
add_executable(uros_modem src/uros/UROSModem.cpp src/DiverMsg.cpp)
add_executable(encoding_v2_test src/test/encoding_v2.cpp)
add_executable(encoding_v3_test src/test/encoding_v3.cpp)
add_executable(packer_test src/test/packer_test.cpp)

## Add cmake target dependencies of the executable/library
## as an example, message headers may need to be generated before nodes
add_dependencies(usbl_comms ${catkin_EXPORTED_TARGETS})
# add_dependencies(usbl_comms_node usbl_comms_generate_messages_cpp)

if (DEFINED catkin_EXPORTED_TARGETS)
 add_dependencies(usbl_comms ${catkin_EXPORTED_TARGETS})
 add_dependencies(uros_comms ${catkin_EXPORTED_TARGETS})
 add_dependencies(uros_modem ${catkin_EXPORTED_TARGETS})
 add_dependencies(divermsg_test ${catkin_EXPORTED_TARGETS})
 add_dependencies(encoding_test ${catkin_EXPORTED_TARGETS})
 add_dependencies(encoding_v2_test ${catkin_EXPORTED_TARGETS})
 add_dependencies(encoding_v3_test ${catkin_EXPORTED_TARGETS})
 add_dependencies(packer_test ${catkin_EXPORTED_TARGETS})
endif()

## Specify libraries to link a library or executable target against
target_link_libraries(usbl_comms
   ${catkin_LIBRARIES}
)

target_link_libraries(caddy_comms
   ${catkin_LIBRARIES}
)

target_link_libraries(uros_comms
   ${catkin_LIBRARIES}
)

target_link_libraries(uros_modem
   ${catkin_LIBRARIES}
)

target_link_libraries(packer_test
   ${catkin_LIBRARIES}
   ${Boost_SERIALIZATION_LIBRARIES}
)

target_link_libraries(encoding_v3_test
   caddy_comms
   ${catkin_LIBRARIES}
   ${Boost_SERIALIZATION_LIBRARIES}
)


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
# install(TARGETS usbl_comms usbl_comms_node
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
catkin_add_gtest(${PROJECT_NAME}-structures-test
  src/caddy/caddy_messages.cpp
  test/test_caddy_comms_structures.cpp
  test/test_comms_structures.cpp)
if(TARGET ${PROJECT_NAME}-structures-test)
   target_link_libraries(${PROJECT_NAME}-structures-test ${Boost_SERIALIZATION_LIBRARIES})
endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
