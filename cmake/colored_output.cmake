function(set_target_colored_output TARGET SCOPE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.9)
            message(STATUS "Enabling colored output for GCC")
            target_compile_options(${TARGET} ${SCOPE}
                -fdiagnostics-color=always
            )
        else()
            message(WARNING
                "GCC version too old (${CMAKE_CXX_COMPILER_VERSION}), colored output requires GCC >= 4.9"
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "Enabling colored output for Clang")
        target_compile_options(${TARGET} ${SCOPE}
            -fcolor-diagnostics
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "Enabling colored output for AppleClang")
        target_compile_options(${TARGET} ${SCOPE}
            -fcolor-diagnostics
        )
    else()
        message(STATUS "Colored output not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()
