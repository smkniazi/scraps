project (ndbapiwrapper_test)

cmake_minimum_required (VERSION 3.5.0)

set (CMAKE_CXX_STANDARD 17)

include_directories (${CMAKE_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../lib)

if(NOT NDB_DIR)
  set(NDB_DIR "/usr/local/mysql")
  message(STATUS "NDB_DIR was not defined. Default: ${NDB_DIR} ")
endif()

include_directories(${NDB_DIR}/include ${NDB_DIR}/include/storage/ndb ${NDB_DIR}/include/storage/ndb/ndbapi ${NDB_DIR}/include/storage/ndb/mgmapi /usr/lib64)
link_directories(${NDB_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/../lib/build)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wformat-security  -Woverloaded-virtual -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-switch -O3 -DDBUG_OFF" )

file(GLOB SOURCE ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(ndbapiwrapper_test ${SOURCE})

target_link_libraries(ndbapiwrapper_test ndbclient ndbapiwrapper pthread)
