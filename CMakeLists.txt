#This file is part of CS framework.
#
#CS framework is free software: you can redistribute it and/or modify
#it under the terms of the GNU Lesser General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#CS framework is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU Lesser General Public License for more details.
#
#You should have received a copy of the GNU Lesser General Public License
#along with CS framework.  If not, see <http://www.gnu.org/licenses/>.


cmake_minimum_required (VERSION 2.6)
project (Server)
include_directories ("${PROJECT_SOURCE_DIR}/log_util")
include_directories ("${PROJECT_SOURCE_DIR}/common")
include_directories ("${PROJECT_SOURCE_DIR}/appserver")
add_executable(server
               main.cpp
               appserver/appserver.cpp
               appserver/appcmdparser.cpp
               appserver/servant.cpp
               log_util/LogMacros.cpp)
find_package(Threads)
target_link_libraries(server
                      ${CMAKE_THREAD_LIBS_INIT})
install (TARGETS server DESTINATION bin)

# configure a header file to pass some of the CMake settings
# to the source code
set(VERSIONMAJOR 1)
set(VERSIONMINOR 0)
set(VERSIONPATCH 0)
set(VERSION \"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONPATCH}\")
set(MAINTAINER \"tatwah.mui@gmail.com\")
configure_file (
  "${PROJECT_SOURCE_DIR}/ServerConfig.h.in"
  "${PROJECT_BINARY_DIR}/ServerConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# should we use our own math functions?
option (DEBUG
        "Debug" ON)
#if (DEBUG)
#    add_definitions(-DDEBUG)
#endif (DEBUG)
