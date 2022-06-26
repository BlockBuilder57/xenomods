add_executable(skyline
		main.cpp
		module.cpp
		operator.cpp

		bf2mods/mm/math_types.cpp

		bf2mods/bdat_randomizer.cpp
		bf2mods/bf2logger.cpp
		bf2mods/camera_tools.cpp
		bf2mods/debug_stuff.cpp
		bf2mods/party_selector.cpp
		bf2mods/player_movement.cpp
		bf2mods/menu_viewer.cpp
		bf2mods/plugin.cpp
		bf2mods/plugin_main.cpp

		bf2mods/stuff/utils/debug_util.cpp
		bf2mods/stuff/utils/util.cpp

		crt0.s
		crti.c
		cxa.c


		skyline/inlinehook/And64InlineHook.cpp
		skyline/inlinehook/controlledpages.cpp
		skyline/inlinehook/memcpy_controlled.cpp

		# deprecated sources
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

add_dependencies(skyline __bf2mods_gittag)

set_target_properties(skyline PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON

		# We need to trick cmake into making this generate PIC code and link as a
		# PIC binary, because we can't use add_library(xxx SHARED).
		# This is ugly, but it SHOULD work.
		COMPILE_FLAGS "-fPIC -enable-libstdcxx-allocator=new"
		LINK_FLAGS "-fPIC -specs=${PROJECT_BINARY_DIR}/switch.specs -Wl,-Map,bf2mods.map -Wl,--version-script=${PROJECT_SOURCE_DIR}/linkerscripts/exported.txt -nodefaultlibs -Wl,-init=__custom_init -Wl,-fini=__custom_fini -Wl,--export-dynamic -L ${DEVKITPRO}/portlibs/switch/lib -L ${DEVKITPRO}/libnx/lib"
)

target_include_directories(skyline PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(skyline PUBLIC ${PROJECT_BINARY_DIR}/)

#target_link_libraries(skyline PUBLIC glm::glm)

# NPDM/NSO output.
add_npdm(skyline ${PROJECT_SOURCE_DIR}/npdm/bf2.json main)
add_nso_target(skyline)