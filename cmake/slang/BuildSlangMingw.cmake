if(NOT DEFINED SLANG_SOURCE_DIR)
    message(FATAL_ERROR "SLANG_SOURCE_DIR is required.")
endif()

if(NOT DEFINED SLANG_MAKE_PROGRAM)
    message(FATAL_ERROR "SLANG_MAKE_PROGRAM is required.")
endif()

if(NOT DEFINED SLANG_TOOL_PREFIX)
    message(FATAL_ERROR "SLANG_TOOL_PREFIX is required.")
endif()

if(NOT DEFINED SLANG_RC_DIR)
    message(FATAL_ERROR "SLANG_RC_DIR is required.")
endif()

set(slang_mkmake "${SLANG_SOURCE_DIR}/src/mkfiles/mkmake.exe")
if(NOT EXISTS "${slang_mkmake}")
    message(FATAL_ERROR
        "Bundled S-Lang fallback expected mkmake.exe at '${slang_mkmake}', but it was not found."
    )
endif()

function(_run_mkmake input_file output_file)
    execute_process(
        COMMAND "${slang_mkmake}" WIN32 MINGW32 DLL
        WORKING_DIRECTORY "${SLANG_SOURCE_DIR}"
        INPUT_FILE "${input_file}"
        OUTPUT_FILE "${output_file}"
        RESULT_VARIABLE mkmake_result
    )
    if(NOT mkmake_result EQUAL 0)
        message(FATAL_ERROR
            "Bundled S-Lang fallback failed while generating '${output_file}' with mkmake.exe."
        )
    endif()
endfunction()

function(_run_make working_subdir)
    execute_process(
        COMMAND
            "${CMAKE_COMMAND}" -E env
            TOOL_PREFIX=${SLANG_TOOL_PREFIX}
            CFLAGS=-std=gnu89
            LFLAGS=-static-libgcc
            "${SLANG_MAKE_PROGRAM}"
        WORKING_DIRECTORY "${SLANG_SOURCE_DIR}/${working_subdir}"
        COMMAND_ECHO STDOUT
        RESULT_VARIABLE slang_make_result
    )

    if(NOT slang_make_result EQUAL 0)
        message(FATAL_ERROR
            "Bundled S-Lang build failed in '${working_subdir}' with exit code ${slang_make_result}."
        )
    endif()
endfunction()

function(_embed_library_resource)
    string(REGEX REPLACE "[/\\\\]+$" "" slang_tool_dir "${SLANG_TOOL_PREFIX}")

    find_program(slang_gcc_program NAMES gcc HINTS "${slang_tool_dir}" NO_CACHE NO_DEFAULT_PATH)
    find_program(slang_windres_program NAMES windres HINTS "${slang_tool_dir}" NO_CACHE NO_DEFAULT_PATH)

    if(NOT slang_gcc_program)
        message(FATAL_ERROR
            "Bundled S-Lang fallback could not find gcc in '${slang_tool_dir}'."
        )
    endif()

    if(NOT slang_windres_program)
        message(FATAL_ERROR
            "Bundled S-Lang fallback could not find windres in '${slang_tool_dir}'."
        )
    endif()

    set(slang_object_dir "${SLANG_SOURCE_DIR}/src/gw32objs")
    set(slang_resource_source "${SLANG_RC_DIR}/libslang_private.rc")
    set(slang_resource_object "${slang_object_dir}/libslang_private.res.o")
    set(slang_runtime_library "${slang_object_dir}/libslang.dll")
    set(slang_import_library "${slang_object_dir}/libslang.a")

    if(NOT EXISTS "${slang_resource_source}")
        message(FATAL_ERROR
            "Bundled S-Lang fallback expected library resource file at '${slang_resource_source}', but it was not found."
        )
    endif()

    file(GLOB slang_library_objects "${slang_object_dir}/*.o")
    list(FILTER slang_library_objects EXCLUDE REGEX [[/\\]libslang_private\.res\.o$]])
    list(SORT slang_library_objects)

    execute_process(
        COMMAND
            "${slang_windres_program}"
            -i "${slang_resource_source}"
            -o "${slang_resource_object}"
        WORKING_DIRECTORY "${SLANG_SOURCE_DIR}/src"
        COMMAND_ECHO STDOUT
        RESULT_VARIABLE slang_windres_result
        OUTPUT_VARIABLE slang_windres_stdout
        ERROR_VARIABLE slang_windres_stderr
    )
    if(NOT slang_windres_result EQUAL 0)
        message(FATAL_ERROR
            "Bundled S-Lang fallback failed while compiling '${slang_resource_source}' with windres.\n"
            "${slang_windres_stdout}${slang_windres_stderr}"
        )
    endif()

    execute_process(
        COMMAND
            "${slang_gcc_program}"
            -shared
            -static-libgcc
            -o "${slang_runtime_library}"
            ${slang_library_objects}
            "${slang_resource_object}"
            "-Wl,--out-implib=${slang_import_library}"
        WORKING_DIRECTORY "${SLANG_SOURCE_DIR}/src"
        COMMAND_ECHO STDOUT
        RESULT_VARIABLE slang_relink_result
        OUTPUT_VARIABLE slang_relink_stdout
        ERROR_VARIABLE slang_relink_stderr
    )
    if(NOT slang_relink_result EQUAL 0)
        message(FATAL_ERROR
            "Bundled S-Lang fallback failed while relinking libslang.dll with the Windows resource object.\n"
            "${slang_relink_stdout}${slang_relink_stderr}"
        )
    endif()
endfunction()

file(COPY_FILE
    "${SLANG_SOURCE_DIR}/src/slconfig.h"
    "${SLANG_SOURCE_DIR}/src/config.h"
    ONLY_IF_DIFFERENT
)

# The upstream MinGW makefiles assume the object directories already exist.
# Create them explicitly so the build does not depend on make's prerequisite
# ordering across directories.
file(MAKE_DIRECTORY
    "${SLANG_SOURCE_DIR}/src/gw32objs"
    "${SLANG_SOURCE_DIR}/slsh/gw32objs"
)

_run_mkmake("${SLANG_SOURCE_DIR}/src/mkfiles/makefile.all" "${SLANG_SOURCE_DIR}/src/Makefile")
_run_mkmake("${SLANG_SOURCE_DIR}/slsh/mkfiles/makefile.all" "${SLANG_SOURCE_DIR}/slsh/Makefile")
_run_mkmake("${SLANG_SOURCE_DIR}/modules/mkfiles/makefile.all" "${SLANG_SOURCE_DIR}/modules/Makefile")

_run_make("src")
_embed_library_resource()
_run_make("slsh")
_run_make("modules")
