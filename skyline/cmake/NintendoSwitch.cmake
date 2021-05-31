set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR armv8-a)
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set(CMAKE_STATIC_LIBRARY_SUFFIX_C "${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(CMAKE_STATIC_LIBRARY_SUFFIX_CXX "${CMAKE_STATIC_LIBRARY_SUFFIX}")

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C "${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_EXECUTABLE_SUFFIX_CXX "${CMAKE_EXECUTABLE_SUFFIX}")

# allow CMake projects to detect NX compilation
set(NX TRUE)


macro(msys_to_cmake MsysPath ResultingPath)
	if(WIN32)
        # Instead of a hacky regex, we can ask msys2 nicely since it comes with a convenient
        # built in utility to do path conversion for us.
        # Yes, this means I unfortunately have to seed the path here,
        # but i don't think there's any other way to (other than shipping cygpath in tree which just doesnt make sense)
        execute_process(
                COMMAND C:/devkitPro/msys2/usr/bin/cygpath.exe -m ${MsysPath}
                OUTPUT_VARIABLE ${ResultingPath}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )
	else()
		set(${ResultingPath} "${MsysPath}")
	endif()
endmacro()

# Setup paths
msys_to_cmake("$ENV{DEVKITPRO}" DEVKITPRO)
if(NOT IS_DIRECTORY ${DEVKITPRO})
	message(FATAL_ERROR "${DEVKITPRO} devkitPro not found, please install to compile NX applications")
endif()

# ??? devkitA64 does not have a environment variable
# (at least, not using the Windows installer)
# change out when this is fixed've by the dkp team
# 2021 update: they haven't done anything to fix it
set(DEVKITA64 "${DEVKITPRO}/devkitA64")
if(NOT IS_DIRECTORY ${DEVKITA64})
	message(FATAL_ERROR "devkitA64 not found, please install to compile NX applications")
endif()


if(WIN32)
    set(CMAKE_C_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc.exe")
    set(CMAKE_CXX_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-g++.exe")
    set(CMAKE_ASM_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc.exe")
    set(CMAKE_AR "${DEVKITA64}/bin/aarch64-none-elf-gcc-ar.exe")
    set(CMAKE_RANLIB "${DEVKITA64}/bin/aarch64-none-elf-gcc-ranlib.exe")
else()
    set(CMAKE_C_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc")
    set(CMAKE_CXX_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-g++")
    set(CMAKE_ASM_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc")
    set(CMAKE_AR "${DEVKITA64}/bin/aarch64-none-elf-gcc-ar")
    set(CMAKE_RANLIB "${DEVKITA64}/bin/aarch64-none-elf-gcc-ranlib")
endif()

set(CMAKE_FIND_ROOT_PATH ${DEVKITA64} ${DEVKITPRO})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Shared libs not available for target")

set(ARCH "-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIC -ftls-model=local-exec")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp -g ${ARCH}")
set(CMAKE_C_FLAGS "${ARCH} -D__SWITCH__ -D__SKYLINE__ -fomit-frame-pointer -ffunction-sections -isystem ${DEVKITPRO}/libnx/include -I${PROJECT_SOURCE_DIR}/libs/libeiffel/include")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -fpermissive")
set(CMAKE_EXE_LINKER_FLAGS "-specs=${PROJECT_SOURCE_DIR}/linkerscripts/switch.specs -Wl,-T,${PROJECT_SOURCE_DIR}/linkerscripts/application.ld,${PROJECT_SOURCE_DIR}/linkerscripts/syms.ld -Wl,--version-script=${PROJECT_SOURCE_DIR}/exported.txt -Wl,-init=__custom_init -Wl,-fini=__custom_fini -Wl,--export-dynamic -nodefaultlibs")


