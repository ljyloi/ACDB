cmake_minimum_required (VERSION 2.8)

project(ACDB)

include_directories(
  ${PROJECT_SOURCE_DIR}/include/
)

add_compile_options(-std=c++11)
message(STATUS "-std=c++11")  

aux_source_directory(
  ${PROJECT_SOURCE_DIR}/src/recordmanager
  RM
)

set(RM_TEST
  ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${RM}
  ${PROJECT_SOURCE_DIR}/src/utils/MyBitMap.cpp
)

add_executable(RM_TEST ${RM_TEST})
