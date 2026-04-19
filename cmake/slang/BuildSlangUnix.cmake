if(NOT DEFINED SLANG_SOURCE_DIR)
    message(FATAL_ERROR "SLANG_SOURCE_DIR is required.")
endif()

if(NOT DEFINED SLANG_MAKE_PROGRAM)
    message(FATAL_ERROR "SLANG_MAKE_PROGRAM is required.")
endif()

execute_process(
    COMMAND "${SLANG_MAKE_PROGRAM}"
            -j1
    WORKING_DIRECTORY "${SLANG_SOURCE_DIR}"
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE slang_build_result
)

if(NOT slang_build_result EQUAL 0)
    message(FATAL_ERROR "Bundled S-Lang build failed with exit code ${slang_build_result}.")
endif()
