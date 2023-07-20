cmake_minimum_required(VERSION 3.19)

if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "NO NO NO NO NO. Please use a distinct binary directory.")
endif()

project(xenomods
		DESCRIPTION ""
		LANGUAGES C CXX ASM
		)

find_package(Python3 REQUIRED
		COMPONENTS Interpreter)

option(XENOMODS_ENABLE_FTP OFF "enable ftp transfer")

# If no codename was set, default to bf2.
if(" ${XENOMODS_CODENAME}" STREQUAL " ")
	set(XENOMODS_CODENAME "bf2")
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/vendor/imgui-xeno/cmake)
include(MakeNso)
include(FpicExternal)

# Generate switch.specs for this build
set(SKYLAUNCH_LINKERSCRIPTS_DIR "${PROJECT_SOURCE_DIR}/linkerscripts")
# ugh
msys_to_cmake("${SKYLAUNCH_LINKERSCRIPTS_DIR}" SKYLAUNCH_LINKERSCRIPTS_DIR)
configure_file(linkerscripts/switch.specs.in ${PROJECT_BINARY_DIR}/switch.specs)

# Generate version
set(VERSION_FILENAME gitversion.h)

add_custom_target(__xenomods_gittag
		COMMAND $<TARGET_FILE:Python3::Interpreter> ${PROJECT_SOURCE_DIR}/scripts/gittag.py ${PROJECT_BINARY_DIR}/${VERSION_FILENAME}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		SOURCES ${PROJECT_BINARY_DIR}/${VERSION_FILENAME}
		)
set_source_files_properties(${PROJECT_BINARY_DIR}/${VERSION_FILENAME} PROPERTIES GENERATED TRUE)

# Vendor libraries
add_subdirectory(vendor/glm)
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/tomlpp)

set(IMGUI_XENO_EXTERNAL_TOOLCHAIN TRUE)
add_subdirectory(vendor/imgui-xeno)

#include_directories("vendor/imgui-xeno/include")
#include_directories("vendor/imgui-xeno/src")
#include_directories("vendor/imgui-xeno/extern/imgui")

add_subdirectory(src)

message(STATUS "Xenomods Buildtype: ${CMAKE_BUILD_TYPE}")
message(STATUS "Xenomods Codename: ${XENOMODS_CODENAME}")
