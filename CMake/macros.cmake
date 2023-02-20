function(SetMaxWarningLevel Project)
    if(MSVC)
        target_compile_options(${Project} PRIVATE /W4 /WX)
    else()
        target_compile_options(${Project} PRIVATE
        #[[ Interesting
            -Wall 
            -Wextra 
            -Wshadow 
            -Wnon-virtual-dtor 
            -Wpedantic 
            -Wconversion 
            -Wpedantic 
            -Werror
            -pedantic
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wsign-conversion
            -Wmisleading-indentation
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
        ]]#
            -Werror
            -Weverything
            -Wno-pre-c++17-compat
            -Wno-c++98-compat
            -Wno-weak-vtables
            -Wno-padded
            -Wno-exit-time-destructors
            -Wno-global-constructors
            -Wno-ctad-maybe-unsupported
        )
    endif()
endfunction(SetMaxWarningLevel)

function(SetLucidVersion Project)
    target_compile_definitions(${Project} PRIVATE LUCID_RENDER_VERSION="${LUCID_RENDER_VERSION}")
endfunction(SetLucidVersion)
