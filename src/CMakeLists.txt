add_executable(xenomods

		xenomods/main.cpp

		# UTILITY
		xenomods/FunctionalHooks.cpp
		xenomods/NnFile.cpp

		xenomods/modules/UpdatableModule.cpp

		xenomods/State.cpp
		xenomods/Logger.cpp
		xenomods/HidInput.cpp
		xenomods/Version.cpp

		xenomods/menu/Menu.cpp
		xenomods/menu/Section.cpp
		xenomods/menu/Option.cpp

		xenomods/modules/DebugStuff.cpp
		xenomods/modules/CameraTools.cpp
		xenomods/modules/RenderingControls.cpp
		xenomods/modules/BattleCheats.cpp
		xenomods/modules/PartyEditor.cpp
		xenomods/modules/PlayerMovement.cpp
		xenomods/modules/FrameratePatch.cpp
		xenomods/modules/FileDetours.cpp
		xenomods/modules/BdatOverride.cpp
		xenomods/modules/BdatRandomizer.cpp
		xenomods/modules/bf2/EventDebugUtils.cpp
		xenomods/modules/bfsw/AppletRedirect.cpp
		xenomods/modules/bfsw/ResourceManager.cpp

        xenomods/stuff/utils/debug_util.cpp
		xenomods/stuff/utils/util.cpp


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

add_dependencies(xenomods __xenomods_gittag)

add_custom_target(version_toucher
		COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/xenomods/Version.cpp)

add_dependencies(xenomods version_toucher)

set_target_properties(xenomods PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON

		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}

		# We need to trick cmake into making this generate PIC code and link as a
		# PIC binary, because we can't use add_library(xxx SHARED).
		# This is ugly, but it SHOULD work.
		COMPILE_FLAGS "-fPIC -enable-libstdcxx-allocator=new"
		LINK_FLAGS "-fPIC -specs=${PROJECT_BINARY_DIR}/switch.specs -Wl,-Map,xenomods.map -Wl,--version-script=${PROJECT_SOURCE_DIR}/linkerscripts/exported.txt -nodefaultlibs -Wl,-init=__custom_init -Wl,-fini=__custom_fini -Wl,--export-dynamic -L ${DEVKITPRO}/portlibs/switch/lib -L ${DEVKITPRO}/libnx/lib"
)

target_include_directories(xenomods PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(xenomods PUBLIC ${PROJECT_BINARY_DIR}/)

# not entirely sure what this is about, but
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOML_FLOAT_CHARCONV=1 -DTOML_ENABLE_FLOAT16=0")

target_link_libraries(xenomods PUBLIC
		tomlplusplus::tomlplusplus
		fmt::fmt-header-only
		glm::glm
		)

target_compile_definitions(xenomods PUBLIC
	__XENOMODS_CODENAME_${XENOMODS_CODENAME}
	_XENOMODS_CODENAME_STR="${XENOMODS_CODENAME}"
)

# NPDM/NSO output.
add_npdm(xenomods ${PROJECT_SOURCE_DIR}/npdm/${XENOMODS_CODENAME}.json main-unpatched)
add_nso_target(xenomods)
patch_npdm(main-unpatched main)

# FTP transfer

if(NOT " ${XENOMODS_SWITCH_IP}" STREQUAL " ")
	message(STATUS "Enabling FTP transfer")

	add_custom_command(TARGET xenomods
			POST_BUILD
			COMMAND $<TARGET_FILE:Python3::Interpreter> ${PROJECT_SOURCE_DIR}/scripts/send_over_ftp.py --switchip ${XENOMODS_SWITCH_IP} --json ${PROJECT_SOURCE_DIR}/npdm/${XENOMODS_CODENAME}.json
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			USES_TERMINAL
			VERBATIM
			COMMENT "Sending xenomods to console"
			)
endif()
