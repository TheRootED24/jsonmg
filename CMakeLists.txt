cmake_minimum_required(VERSION 3.15)
mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

project(jsonmg)


#-------------------------------------------------------------------------------
# Project / Files
#-------------------------------------------------------------------------------

set(PROJECT_NAME jsonmg)

# Version
set(VERSION_MAJ      1)
set(VERSION_MIN      0)
set(VERSION_CL       0)
set(VERSION_PL       0)
set(VERSION_BN       0)
set(VERSION ${VERSION_MAJ}.${VERSION_MIN}.${VERSION_PL})

string(TIMESTAMP RELEASE_DATE)
message(STATUS "RELEASE_DATE: ${RELEASE_DATE}")
message(STATUS "VERSION:      ${VERSION}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/jsonmg.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/src/jsonmg.h )


set(CMAKE_BUILD_TYPE "Debug")

find_package(Lua5.15 REQUIRED)
find_package(mongoose REQUIRED)

add_subdirectory(src)
