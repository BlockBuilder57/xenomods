cmake_minimum_required(VERSION 3.10)

if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "NO NO NO NO NO. Please use a distinct binary directory.")
endif()

project(skyline)


# Enable languages used in the project
enable_language(C)
enable_language(CXX)
enable_language(ASM)

option(BF2MODS_ENABLE_FTP OFF "enable ftp transfer")

# If no codename was set default to bf2
if(" ${BF2MODS_CODENAME}" STREQUAL " ")
	set(BF2MODS_CODENAME "bf2")
endif()


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MakeNso)

# Generate switch.specs for this build
set(SKYLINE_LINKERSCRIPTS_DIR "${PROJECT_SOURCE_DIR}/linkerscripts")
configure_file(linkerscripts/switch.specs.in ${PROJECT_BINARY_DIR}/switch.specs)

# Generate version
add_custom_target(__bf2mods_gittag COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/scripts/gittag.cmake WORKING_DIRECTORY ${PROJECT_BINARY_DIR} SOURCES ${PROJECT_BINARY_DIR}/version.h)
set_source_files_properties(${PROJECT_BINARY_DIR}/version.h PROPERTIES GENERATED TRUE)

# Vendor libraries
add_subdirectory(vendor/glm)
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/tomlpp)

add_subdirectory(src)

message(STATUS "Bf2mods Buildtype: ${CMAKE_BUILD_TYPE}")
message(STATUS "Bf2mods Codename: ${BF2MODS_CODENAME}")