function(SetMaxWarningLevel Project)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE
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
