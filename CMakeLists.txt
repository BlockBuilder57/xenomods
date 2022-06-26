cmake_minimum_required(VERSION 3.10)
project(skyline)

# Enable languages used in the project
enable_language(C)
enable_language(CXX)
enable_language(ASM)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MakeNso)

set(SKYLINE_LINKERSCRIPTS_DIR "${PROJECT_SOURCE_DIR}/linkerscripts")
configure_file(${PROJECT_SOURCE_DIR}/linkerscripts/switch.specs.in ${PROJECT_BINARY_DIR}/switch.specs)


add_subdirectory(src)
