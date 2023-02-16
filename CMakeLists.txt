cmake_minimum_required(VERSION 3.19)

if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "NO NO NO NO NO. Please use a distinct binary directory.")
endif()

project(bf2mods
		DESCRIPTION ""
		LANGUAGES C CXX ASM
		)

find_package(Python3 REQUIRED
		COMPONENTS Interpreter)

option(BF2MODS_ENABLE_FTP OFF "enable ftp transfer")

# If no codename was set, default to bf2.
if(" ${BF2MODS_CODENAME}" STREQUAL " ")
	set(BF2MODS_CODENAME "bf2")
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MakeNso)

# Generate switch.specs for this build
set(SKYLAUNCH_LINKERSCRIPTS_DIR "${PROJECT_SOURCE_DIR}/linkerscripts")
configure_file(linkerscripts/switch.specs.in ${PROJECT_BINARY_DIR}/switch.specs)

# Generate version
set(VERSION_FILENAME gitversion.h)

add_custom_target(__bf2mods_gittag
		COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/gittag.py ${VERSION_FILENAME}
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		SOURCES ${PROJECT_BINARY_DIR}/${VERSION_FILENAME}
		)
set_source_files_properties(${PROJECT_BINARY_DIR}/${VERSION_FILENAME} PROPERTIES GENERATED TRUE)

# Vendor libraries
add_subdirectory(vendor/glm)
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/tomlpp)

add_subdirectory(src)

message(STATUS "Bf2mods Buildtype: ${CMAKE_BUILD_TYPE}")
message(STATUS "Bf2mods Codename: ${BF2MODS_CODENAME}")
