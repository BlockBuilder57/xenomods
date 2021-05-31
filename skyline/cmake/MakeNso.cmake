# 
# Make a NSO from a compiled ELF target
#

if(NOT DEVKITPRO)
	message(FATAL_ERROR "MakeNso needs a established working DEVKITPRO environment variable.")
endif()

find_program(ELF2NSO elf2nso ${DEVKITPRO}/tools/bin)
if(ELF2NSO)
	message(STATUS "elf2nso found at ${ELF2NSO}")
else()
	message(FATAL_ERROR "elf2nso not found")
endif()


find_program(NPDMTOOL npdmtool ${DEVKITPRO}/tools/bin)
if(NPDMTOOL)
	message(STATUS "npdmtool found at ${NPDMTOOL}")
else()
	message(FATAL_ERROR "npdmtool not found")
endif()

function(add_npdm target json_file)
		add_custom_command(TARGET ${target}
				PRE_BUILD
				COMMAND ${NPDMTOOL} ${json_file} ${CMAKE_CURRENT_BINARY_DIR}/main.npdm
				WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
				VERBATIM
				COMMENT "Making NPDM ${CMAKE_CURRENT_BINARY_DIR}/main.npdm from ${json_file}"
		)
endfunction()

function(add_nso_target target)
	get_filename_component(target_we ${target} NAME_WE)
	add_custom_command(TARGET ${target}
				POST_BUILD
				COMMAND ${ELF2NSO} ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.elf ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.nso
				USES_TERMINAL
				VERBATIM
				COMMENT "Running Elf2nso on ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.elf to make ${target_we}.nso"
    )
	#add_dependencies
endfunction()
	