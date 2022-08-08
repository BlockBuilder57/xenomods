add_executable(bf2mods
		main.cpp
		module.cpp
		operator.cpp

		bf2mods/UpdatableModule.cpp

        bf2mods/engine/mm/math_types.cpp

		bf2mods/state.cpp
		bf2mods/plugin_main.cpp
		bf2mods/debug_stuff.cpp
		bf2mods/bf2logger.cpp
		bf2mods/camera_tools.cpp
		bf2mods/menu_viewer.cpp
		bf2mods/player_movement.cpp
		bf2mods/bdat_randomizer.cpp
		bf2mods/party_selector.cpp

        bf2mods/stuff/utils/debug_util.cpp
		bf2mods/stuff/utils/util.cpp

		# runtime source (which we use to get executed)
		crt0.s
		crti.c
		cxa.c

		# hook source code
		skyline/inlinehook/And64InlineHook.cpp
		skyline/inlinehook/controlledpages.cpp
		skyline/inlinehook/memcpy_controlled.cpp

		# deprecated sources which will slowly be removed
		nvn/pfnc.cpp

		skyline/efl/service.cpp
		skyline/logger/KernelLogger.cpp
		skyline/logger/Logger.cpp
		skyline/logger/SdLogger.cpp
		skyline/logger/TcpLogger.cpp
		skyline/utils/SafeQueue.cpp
		skyline/utils/cpputils.cpp
		skyline/utils/ipc.cpp

		skyline/utils/utils.c
		skyline/nx/kernel/condvar.c
		skyline/nx/kernel/detect.c
		skyline/nx/kernel/jit.c
		skyline/nx/kernel/mutex.c
		skyline/nx/kernel/shmem.c
		skyline/nx/kernel/thread.c
		skyline/nx/kernel/virtmem.c
		skyline/nx/runtime/env.c
		skyline/nx/runtime/hosversion.c
		skyline/nx/runtime/init.c
		skyline/nx/sf/sessionmgr.c
		skyline/utils/armutils.s
		skyline/nx/arm/cache.s

		skyline/nx/kernel/svc.s
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

# TODO: I want to use fmt::fmt but it doesn't seem to respect fpic. so for now header only it is
target_link_libraries(bf2mods PUBLIC
		tomlplusplus::tomlplusplus
		fmt::fmt-header-only
		glm::glm
		)

target_compile_definitions(bf2mods PUBLIC
	__BF2MODS_CODENAME_${BF2MODS_CODENAME}
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