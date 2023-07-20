# forces -fPIC into the compile args of an external target
macro(xenomods_external_fpic target)
    set_target_properties(${target} PROPERTIES
            COMPILE_FLAGS "-fPIC -enable-libstdcxx-allocator=new"
            )
endmacro()