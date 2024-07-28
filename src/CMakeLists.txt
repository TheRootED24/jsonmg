cmake_minimum_required(VERSION 3.1...3.27)
 
project(
  jsonmg
  VERSION 1.0
  LANGUAGES C
  )

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(jsonmg SHARED jsonmg.c)
target_link_libraries(jsonmg libmongoose.so)
target_link_libraries(jsonmg liblua.so)
target_link_libraries(jsonmg m)
install(TARGETS jsonmg DESTINATION /usr/lib/lua/jsonmg/)
