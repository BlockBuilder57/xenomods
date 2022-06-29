cmake_minimum_required(VERSION 3.10)
project(skyline)


# Enable languages used in the project
enable_language(C)
enable_language(CXX)
enable_language(ASM)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MakeNso)

# Generate switch.specs for this build
set(SKYLINE_LINKERSCRIPTS_DIR "${PROJECT_SOURCE_DIR}/linkerscripts")
configure_file(linkerscripts/switch.specs.in ${PROJECT_BINARY_DIR}/switch.specs)

# Generate version
add_custom_target(__bf2mods_gittag COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/scripts/gittag.cmake WORKING_DIRECTORY ${PROJECT_BINARY_DIR} SOURCES ${PROJECT_BINARY_DIR}/version.h)
set_source_files_properties(${PROJECT_BINARY_DIR}/version.h PROPERTIES GENERATED TRUE)

add_subdirectory(vendor/glm)

add_subdirectory(src)
