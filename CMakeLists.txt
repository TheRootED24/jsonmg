cmake_minimum_required(VERSION 3.15)
mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

project(jsonmg)


#-------------------------------------------------------------------------------
# Project / Files
#-------------------------------------------------------------------------------

set(PROJECT_NAME jsonmg)

# Version
set(VERSION_MAJ      7)
set(VERSION_MIN      1)
set(VERSION_CL       0)
set(VERSION_PL       4)
set(VERSION_BN       0)
set(VERSION ${VERSION_MAJ}.${VERSION_MIN}.${VERSION_PL})

string(TIMESTAMP RELEASE_DATE)
message(STATUS "RELEASE_DATE: ${RELEASE_DATE}")
message(STATUS "VERSION:      ${VERSION}")

add_subdirectory(src)
