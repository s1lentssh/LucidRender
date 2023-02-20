function(SetMaxWarningLevel Project)
    if(MSVC)
        target_compile_options(${Project} PRIVATE /W4 /WX)
    else()
        target_compile_options(${Project} PRIVATE
            -Wall 
            -Wextra 
            -Wshadow 
            -Wnon-virtual-dtor 
            -Wpedantic 
            -Wconversion 
            -Wpedantic 
            -Werror
        )
    endif()
endfunction(SetMaxWarningLevel)

function(SetLucidVersion Project)
    target_compile_definitions(${Project} PRIVATE LUCID_RENDER_VERSION="${LUCID_RENDER_VERSION}")
endfunction(SetLucidVersion)