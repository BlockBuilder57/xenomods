add_executable(xenomods

		xenomods/main.cpp

		# UTILITY
		xenomods/FunctionalHooks.cpp
		xenomods/NnFile.cpp
		xenomods/ImGuiExtensions.cpp

		xenomods/modules/UpdatableModule.cpp

		xenomods/State.cpp
		xenomods/Logger.cpp
		xenomods/HidInput.cpp
		xenomods/Version.cpp

		xenomods/menu/Menu.cpp
		xenomods/menu/Section.cpp
		xenomods/menu/MenuLog.cpp

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
		xenomods/modules/ResourceManager.cpp
		xenomods/modules/bf2/EventDebugUtils.cpp
		xenomods/modules/bf2/CompatabilityFix.cpp
		xenomods/modules/bfsw/AppletRedirect.cpp

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

		skylaunch/plugin/PluginManager.cpp

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

xenomods_external_fpic(imgui_xeno)
xenomods_external_fpic(fmt)

target_compile_options(imgui_xeno PUBLIC
		-DIMGUI_XENO_LOAD_DEFAULT_FONT=false
		)

target_include_directories(xenomods PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(xenomods PUBLIC ${PROJECT_BINARY_DIR}/)

target_include_directories(xenomods PRIVATE
		${PROJECT_SOURCE_DIR}/vendor/imgui-xeno/include
		${PROJECT_SOURCE_DIR}/vendor/imgui-xeno/extern/imgui
		# :v
		#${PROJECT_SOURCE_DIR}/include/skylaunch
		#${PROJECT_SOURCE_DIR}/vendor/imgui-xeno/src_framework
		#${PROJECT_SOURCE_DIR}/vendor/imgui-xeno/src
		)

# disables architecture unsupported float conversions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOML_FLOAT_CHARCONV=1 -DTOML_ENABLE_FLOAT16=0")

target_link_libraries(xenomods PUBLIC
		tomlplusplus::tomlplusplus
		fmt::fmt
		glm::glm
		imgui_xeno
		magic_enum
		)

target_compile_definitions(xenomods PUBLIC
		__XENOMODS_CODENAME_${XENOMODS_CODENAME}
		_XENOMODS_CODENAME_STR="${XENOMODS_CODENAME}"
		)

# NSO output.
add_nso_target(xenomods)

# "multibuilds" are just one specific build being used multiple times
# we want to loop through the list of them and do all the npdm stuff/ftp uploading per codename

foreach(THIS_CODENAME ${XENOMODS_MULTIBUILD_LIST})
	if (NOT EXISTS ${PROJECT_BINARY_DIR}/${THIS_CODENAME}.npdm)
		# NPDM output.
		add_npdm(xenomods ${PROJECT_SOURCE_DIR}/npdm/${THIS_CODENAME}.json ${THIS_CODENAME}-unpatched)
		patch_npdm(${THIS_CODENAME}-unpatched ${THIS_CODENAME})
	endif()
endforeach()

# FTP transfer
if(NOT " ${XENOMODS_SWITCH_IP}" STREQUAL " ")
	message(STATUS "Enabling FTP transfer")

	add_custom_command(TARGET xenomods
			POST_BUILD
			COMMAND $<TARGET_FILE:Python3::Interpreter> ${PROJECT_SOURCE_DIR}/scripts/send_over_ftp.py --switchip ${XENOMODS_SWITCH_IP} --codename "${XENOMODS_MULTIBUILD_LIST}"
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			USES_TERMINAL
			VERBATIM
			COMMENT "Sending xenomods to console"
			)
endif()