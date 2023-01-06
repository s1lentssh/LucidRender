macro (SetCompileOptions target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE
            -Wall 
            -Wextra 
            -Wshadow 
            -Wnon-virtual-dtor 
            -Wpedantic 
            -Wconversion 
            -Wpedantic 
            -Werror
        )

        if (ENABLE_COVERAGE)
            target_compile_options(${target} PRIVATE
                --coverage 
                -O0
            )
        endif()
endif()
endmacro()