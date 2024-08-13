cmake_minimum_required(VERSION 3.1...3.27)

#-------------------------------------------------------------------------------
# Build Environment
#-------------------------------------------------------------------------------

 add_definitions(-fpic -W -DMG_ENABLE_LINES -DMG_USE_MONGOOSE)
 add_compile_options(-W -Wall -Werror -Wextra)

project(
  jsonmg
  VERSION 1.0
  LANGUAGES C
  )

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(jsonmg SHARED jsonmg.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
target_link_libraries(jsonmg libmgjson.so)
if(MG_USE_MONGOOSE)
target_link_libraries(jsonmg libmongoose.so)
endif()
target_link_libraries(jsonmg liblua.so)
target_link_libraries(jsonmg m)
install(TARGETS jsonmg DESTINATION /usr/lib/lua/jsonmg/)
