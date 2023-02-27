function(SetMaxWarningLevel Project)
    if(WIN32)
        target_compile_options(${Project} PRIVATE /W3)
    elseif(APPLE)
        target_compile_options(${Project} PRIVATE
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
    elseif(UNIX)
        target_compile_options(${Project} PRIVATE
            -Wall 
            -Wextra 
            -Wshadow 
            -Wnon-virtual-dtor 
            -Wpedantic 
            -Wconversion 
            -Werror
            -pedantic
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wsign-conversion
            -Wmisleading-indentation
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast
        )
    endif()
endfunction(SetMaxWarningLevel)

function(SetWindowsVersion Project)
    target_compile_definitions(${Project} PRIVATE _WIN32_WINNT=0x0601)
endfunction(SetWindowsVersion)

function(SetLucidVersion Project)
    target_compile_definitions(${Project} PRIVATE LUCID_RENDER_VERSION="${LUCID_RENDER_VERSION}")
endfunction(SetLucidVersion)

macro(GetWindowsVersion WINDOWS_VERSION)
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" WINDOWS_VERSION ${CMAKE_SYSTEM_VERSION})
    math(EXPR WINDOWS_VERSION "(${CMAKE_MATCH_1} << 8) + ${CMAKE_MATCH_2}" OUTPUT_FORMAT HEXADECIMAL)
endmacro(GetWindowsVersion)
