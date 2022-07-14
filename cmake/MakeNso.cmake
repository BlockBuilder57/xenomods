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

function(add_npdm target json_file output_file)
		add_custom_command(TARGET ${target}
				PRE_BUILD
				COMMAND ${NPDMTOOL} ${json_file} ${output_file}.npdm
				WORKING_DIRECTORY $<TARGET_FILE_DIR:${target}>
				VERBATIM
				COMMENT "Making NPDM ${output_file}.npdm from ${json_file}"
		)
endfunction()

function(add_nso_target target)
	add_custom_command(TARGET ${target}
				POST_BUILD
				COMMAND ${ELF2NSO} $<TARGET_FILE_DIR:${target}>/${target}.elf $<TARGET_FILE_DIR:${target}>/${target}.nso
				USES_TERMINAL
				VERBATIM
				COMMENT "Running Elf2nso on ${target}.elf to make ${target}.nso"
    )
	#add_dependencies
endfunction()
	