set(_SKYLINE_GUNK_SOURCES
	# runtime gunk
	crt0.s
	crti.c
	cxa.c
)

set(_SKYLINE_SOURCES
	main.cpp
	module.cpp
	operator.cpp

	bf2mods/bdat_randomizer.cpp
	bf2mods/debug_stuff.cpp
	bf2mods/party_selector.cpp
	bf2mods/plugin.cpp
	bf2mods/plugin_main.cpp

	bf2mods/stuff/utils/debug_util.cpp
	bf2mods/stuff/utils/util.cpp

	nvn/pfnc.cpp

	skyline/efl/service.cpp
	skyline/inlinehook/And64InlineHook.cpp
	skyline/inlinehook/controlledpages.cpp
	skyline/inlinehook/memcpy_controlled.cpp
	skyline/logger/KernelLogger.cpp
	skyline/logger/Logger.cpp
	skyline/logger/SdLogger.cpp
	skyline/logger/TcpLogger.cpp
	skyline/utils/SafeQueue.cpp
	skyline/utils/cpputils.cpp
	skyline/utils/ipc.cpp
	crti.c
	cxa.c
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
	crt0.s
	skyline/utils/armutils.s
	skyline/nx/arm/cache.s
	skyline/nx/internal.h

	skyline/nx/kernel/svc.s



	)

# this isn't a part of skyline but we're continuing the hacky way of getting it to work for the time being
# but hey, at least i made it techinically a seperate target, right?
set(_BF2MODS_SOURCES
	bf2mods/bdat_randomizer.cpp
	bf2mods/debug_stuff.cpp
	bf2mods/party_selector.cpp
	bf2mods/plugin.cpp
	bf2mods/plugin_main.cpp	
)

#add_library(bf2mods ${_BF2MODS_SOURCES})
#set_property(TARGET bf2mods PROPERTY CXX_STANDARD 20)
#target_include_directories(bf2mods PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bf2mods)
#target_include_directories(bf2mods PUBLIC ${PROJECT_SOURCE_DIR}/../include)
#target_include_directories(bf2mods PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(skyline ${_SKYLINE_SOURCES})

target_include_directories(skyline PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(skyline PUBLIC ${PROJECT_SOURCE_DIR}/../include)
target_include_directories(skyline PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_options(skyline PUBLIC -u malloc)
#target_link_libraries(skyline bf2mods)

# in this house we use c++20
set_property(TARGET skyline PROPERTY CXX_STANDARD 20)

# switch stuff
add_npdm(skyline ${PROJECT_SOURCE_DIR}/bf2.json)
add_nso_target(skyline)
