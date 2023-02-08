add_executable(bf2mods

		bf2mods/main.cpp

		# UTILITY
		bf2mods/NnFile.cpp

		bf2mods/modules/UpdatableModule.cpp

        bf2mods/State.cpp
		bf2mods/Logger.cpp
		bf2mods/HidInput.cpp

		bf2mods/modules/DebugStuff.cpp
		bf2mods/modules/FileDetours.cpp
		bf2mods/modules/BdatOverride.cpp
		bf2mods/modules/CameraTools.cpp
		bf2mods/modules/EventDebugUtils.cpp
		bf2mods/modules/MenuViewer.cpp
		bf2mods/modules/PlayerMovement.cpp
		bf2mods/modules/BdatRandomizer.cpp

        bf2mods/stuff/utils/debug_util.cpp
		bf2mods/stuff/utils/util.cpp


		# Skylaunch (skyline/exlaunch-like illegal breeding experiment, mostly from skyline)
		# TODO: throw most of it (including the nx fork) into another cmakelists
		# 		and then probably keep only the "jackin" sources in here

		# runtime source (which we use to get executed)
		skylaunch/entry/crt0.s
		skylaunch/entry/crti.c
		skylaunch/entry/cxa.c

		skylaunch/entry/main.cpp
		skylaunch/entry/module.cpp
		skylaunch/entry/operator.cpp

		# hook source code
		skylaunch/inlinehook/And64InlineHook.cpp
		skylaunch/inlinehook/controlledpages.cpp

		skylaunch/hookng/Hooks.cpp


		skylaunch/logger/Logger.cpp
		skylaunch/logger/TcpLogger.cpp

		skylaunch/utils/cpputils.cpp
		skylaunch/utils/ipc.cpp
		skylaunch/utils/utils.c

		skylaunch/nx/kernel/condvar.c
		skylaunch/nx/kernel/detect.c
		skylaunch/nx/kernel/jit.c
		skylaunch/nx/kernel/mutex.c
		skylaunch/nx/kernel/shmem.c
		skylaunch/nx/kernel/thread.c
		skylaunch/nx/kernel/virtmem.c
		skylaunch/nx/runtime/env.c
		skylaunch/nx/runtime/hosversion.c
		skylaunch/nx/runtime/init.c
		skylaunch/nx/sf/sessionmgr.c
		skylaunch/utils/armutils.s
		skylaunch/nx/arm/cache.s
		skylaunch/nx/kernel/svc.s
		)

add_dependencies(bf2mods __bf2mods_gittag)

set_target_properties(bf2mods PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON

		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}

		# We need to trick cmake into making this generate PIC code and link as a
		# PIC binary, because we can't use add_library(xxx SHARED).
		# This is ugly, but it SHOULD work.
		COMPILE_FLAGS "-fPIC -enable-libstdcxx-allocator=new"
		LINK_FLAGS "-fPIC -specs=${PROJECT_BINARY_DIR}/switch.specs -Wl,-Map,bf2mods.map -Wl,--version-script=${PROJECT_SOURCE_DIR}/linkerscripts/exported.txt -nodefaultlibs -Wl,-init=__custom_init -Wl,-fini=__custom_fini -Wl,--export-dynamic -L ${DEVKITPRO}/portlibs/switch/lib -L ${DEVKITPRO}/libnx/lib"
)

target_include_directories(bf2mods PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(bf2mods PUBLIC ${PROJECT_BINARY_DIR}/)

# not entirely sure what this is about, but
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOML_ENABLE_FLOAT16=0")

target_link_libraries(bf2mods PUBLIC
		tomlplusplus::tomlplusplus
		fmt::fmt-header-only
		glm::glm
		)

target_compile_definitions(bf2mods PUBLIC
	__BF2MODS_CODENAME_${BF2MODS_CODENAME}
	_BF2MODS_CODENAME_STR="${BF2MODS_CODENAME}"
)

# NPDM/NSO output.
add_npdm(bf2mods ${PROJECT_SOURCE_DIR}/npdm/${BF2MODS_CODENAME}.json main)
add_nso_target(bf2mods)

# FTP transfer

if(NOT " ${BF2MODS_SWITCH_IP}" STREQUAL " ")
	message(STATUS "Enabling FTP transfer")

	add_custom_command(TARGET bf2mods
			POST_BUILD
			COMMAND python ../scripts/send_over_ftp.py --switchip ${BF2MODS_SWITCH_IP} --json ${PROJECT_SOURCE_DIR}/npdm/${BF2MODS_CODENAME}.json
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			USES_TERMINAL
			VERBATIM
			COMMENT "Sending bf2mods to console"
			)
endif()