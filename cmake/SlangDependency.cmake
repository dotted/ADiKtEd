include_guard(GLOBAL)

include(ExternalProject)
include(FindPkgConfig)

function(_set_slang_runtime_properties target_name runtime_location runtime_destination build_rpath install_rpath)
    set_target_properties(
        ${target_name}
        PROPERTIES
            SLANG_RUNTIME_LOCATION "${runtime_location}"
            SLANG_RUNTIME_DESTINATION "${runtime_destination}"
            SLANG_BUILD_RPATH "${build_rpath}"
            SLANG_INSTALL_RPATH "${install_rpath}"
    )
endfunction()

function(_resolve_slang_from_pkg_config out_target out_include_dir out_runtime_file out_runtime_destination out_build_rpath out_install_rpath)
    pkg_check_modules(SLANG QUIET IMPORTED_TARGET slang)
    if(TARGET PkgConfig::SLANG)
        message(STATUS "mapslang: using system S-Lang from pkg-config.")
        set(${out_target} PkgConfig::SLANG PARENT_SCOPE)
        set(${out_include_dir} "${SLANG_INCLUDE_DIRS}" PARENT_SCOPE)
        set(${out_runtime_file} "" PARENT_SCOPE)
        set(${out_runtime_destination} "" PARENT_SCOPE)
        set(${out_build_rpath} "" PARENT_SCOPE)
        set(${out_install_rpath} "" PARENT_SCOPE)
        return()
    endif()

    set(${out_target} "" PARENT_SCOPE)
    set(${out_include_dir} "" PARENT_SCOPE)
    set(${out_runtime_file} "" PARENT_SCOPE)
    set(${out_runtime_destination} "" PARENT_SCOPE)
    set(${out_build_rpath} "" PARENT_SCOPE)
    set(${out_install_rpath} "" PARENT_SCOPE)
endfunction()

function(_resolve_slang_from_paths out_target out_include_dir out_runtime_file out_runtime_destination out_build_rpath out_install_rpath)
    find_path(MAPSLANG_SLANG_INCLUDE_DIR NAMES slang.h)
    find_library(MAPSLANG_SLANG_LIBRARY NAMES slang libslang)
    if(MAPSLANG_SLANG_INCLUDE_DIR AND MAPSLANG_SLANG_LIBRARY)
        message(STATUS "mapslang: using system S-Lang from direct library/header discovery.")
        add_library(slang_dependency_found UNKNOWN IMPORTED GLOBAL)
        set_target_properties(
            slang_dependency_found
            PROPERTIES
                IMPORTED_LOCATION "${MAPSLANG_SLANG_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${MAPSLANG_SLANG_INCLUDE_DIR}"
        )
        set(${out_target} slang_dependency_found PARENT_SCOPE)
        set(${out_include_dir} "${MAPSLANG_SLANG_INCLUDE_DIR}" PARENT_SCOPE)
        set(${out_runtime_file} "" PARENT_SCOPE)
        set(${out_runtime_destination} "" PARENT_SCOPE)
        set(${out_build_rpath} "" PARENT_SCOPE)
        set(${out_install_rpath} "" PARENT_SCOPE)
        return()
    endif()

    set(${out_target} "" PARENT_SCOPE)
    set(${out_include_dir} "" PARENT_SCOPE)
    set(${out_runtime_file} "" PARENT_SCOPE)
    set(${out_runtime_destination} "" PARENT_SCOPE)
    set(${out_build_rpath} "" PARENT_SCOPE)
    set(${out_install_rpath} "" PARENT_SCOPE)
endfunction()

function(_resolve_slang_external_download_args out_args)
    if(MAPSLANG_SLANG_SOURCE STREQUAL "git")
        set(download_args
            GIT_REPOSITORY git://git.jedsoft.org/git/slang.git
            GIT_TAG v2.3.2
            GIT_SHALLOW TRUE
        )
    else()
        set(download_args
            URL https://www.jedsoft.org/releases/slang/slang-2.3.2.tar.bz2
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
    endif()

    set(${out_args} ${download_args} PARENT_SCOPE)
endfunction()

function(_configure_slang_mingw_fallback out_target out_include_dir out_runtime_file out_runtime_destination out_build_rpath out_install_rpath)
    set(slang_prefix "${CMAKE_BINARY_DIR}/_deps/slang")
    set(slang_source_dir "${slang_prefix}/src/slang")
    set(slang_stage_dir "${slang_prefix}/install")
    set(slang_stage_include_dir "${slang_stage_dir}/include")
    set(slang_stage_lib_dir "${slang_stage_dir}/${CMAKE_INSTALL_LIBDIR}")
    set(slang_stage_bin_dir "${slang_stage_dir}/${CMAKE_INSTALL_BINDIR}")
    set(slang_stage_import_library "${slang_stage_lib_dir}/libslang.a")
    set(slang_stage_runtime_library "${slang_stage_bin_dir}/libslang.dll")
    set(slang_patch_dir "${PROJECT_SOURCE_DIR}/cmake/slang/mingw")

    file(MAKE_DIRECTORY "${slang_stage_include_dir}" "${slang_stage_lib_dir}" "${slang_stage_bin_dir}")

    get_filename_component(slang_compiler_dir "${CMAKE_C_COMPILER}" DIRECTORY)
    set(slang_tool_prefix "${slang_compiler_dir}/")

    get_filename_component(slang_toolchain_root "${slang_compiler_dir}" DIRECTORY)
    get_filename_component(slang_msys_root "${slang_toolchain_root}" DIRECTORY)

    unset(slang_build_make_program CACHE)

    if(CMAKE_MAKE_PROGRAM)
        set(slang_build_make_program "${CMAKE_MAKE_PROGRAM}")
    else()
        find_program(
            slang_build_make_program
            NAMES mingw32-make make gmake
            HINTS "${slang_compiler_dir}" "${slang_msys_root}/usr/bin"
            NO_CACHE
        )
    endif()
    if(NOT slang_build_make_program)
        message(FATAL_ERROR
            "mapslang: bundled S-Lang fallback requires a make program for the MinGW toolchain.\n"
            "Looked near: ${slang_compiler_dir} and ${slang_msys_root}/usr/bin"
        )
    endif()

    _resolve_slang_external_download_args(slang_download_args)

    message(STATUS "mapslang: using bundled S-Lang fallback (MinGW driver).")

    ExternalProject_Add(
        slang_external
        PREFIX "${slang_prefix}"
        SOURCE_DIR "${slang_source_dir}"
        ${slang_download_args}
        CONFIGURE_COMMAND ""
        PATCH_COMMAND ""
        BUILD_COMMAND
            ${CMAKE_COMMAND}
            -DSLANG_SOURCE_DIR=${slang_source_dir}
            -DSLANG_MAKE_PROGRAM=${slang_build_make_program}
            -DSLANG_TOOL_PREFIX=${slang_tool_prefix}
            -DSLANG_RC_DIR=${slang_patch_dir}
            -P ${PROJECT_SOURCE_DIR}/cmake/slang/BuildSlangMingw.cmake
        INSTALL_COMMAND
            ${CMAKE_COMMAND}
            -DSLANG_SOURCE_DIR=${slang_source_dir}
            -DSLANG_STAGE_DIR=${slang_stage_dir}
            -P ${PROJECT_SOURCE_DIR}/cmake/slang/StageSlangMingw.cmake
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS "${slang_stage_import_library}" "${slang_stage_runtime_library}" "${slang_stage_include_dir}/slang.h"
    )

    add_library(slang_dependency_fetched SHARED IMPORTED GLOBAL)
    set_target_properties(
        slang_dependency_fetched
        PROPERTIES
            IMPORTED_IMPLIB "${slang_stage_import_library}"
            IMPORTED_LOCATION "${slang_stage_runtime_library}"
            INTERFACE_INCLUDE_DIRECTORIES "${slang_stage_include_dir}"
    )
    _set_slang_runtime_properties(
        slang_dependency_fetched
        "${slang_stage_runtime_library}"
        "${CMAKE_INSTALL_BINDIR}"
        ""
        ""
    )
    add_dependencies(slang_dependency_fetched slang_external)

    set(${out_target} slang_dependency_fetched PARENT_SCOPE)
    set(${out_include_dir} "${slang_stage_include_dir}" PARENT_SCOPE)
    set(${out_runtime_file} "${slang_stage_runtime_library}" PARENT_SCOPE)
    set(${out_runtime_destination} "${CMAKE_INSTALL_BINDIR}" PARENT_SCOPE)
    set(${out_build_rpath} "" PARENT_SCOPE)
    set(${out_install_rpath} "" PARENT_SCOPE)
endfunction()

function(_configure_slang_unix_fallback out_target out_include_dir out_runtime_file out_runtime_destination out_build_rpath out_install_rpath)
    set(slang_prefix "${CMAKE_BINARY_DIR}/_deps/slang")
    set(slang_source_dir "${slang_prefix}/src/slang")
    set(slang_stage_dir "${slang_prefix}/install")
    set(slang_stage_include_dir "${slang_stage_dir}/include")
    set(slang_stage_lib_dir "${slang_stage_dir}/${CMAKE_INSTALL_LIBDIR}")
    set(slang_stage_library "${slang_stage_lib_dir}/libslang${CMAKE_SHARED_LIBRARY_SUFFIX}")

    file(MAKE_DIRECTORY "${slang_stage_include_dir}" "${slang_stage_lib_dir}")

    find_program(slang_make_program NAMES gmake make)
    if(NOT slang_make_program)
        message(FATAL_ERROR
            "mapslang: bundled S-Lang fallback requires a make program on Unix-like platforms."
        )
    endif()

    _resolve_slang_external_download_args(slang_download_args)

    if(APPLE)
        set(slang_install_rpath "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
    else()
        set(slang_install_rpath "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
    endif()

    message(STATUS "mapslang: using bundled S-Lang fallback (Unix driver).")

    ExternalProject_Add(
        slang_external
        PREFIX "${slang_prefix}"
        SOURCE_DIR "${slang_source_dir}"
        ${slang_download_args}
        CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -E env
            CC=${CMAKE_C_COMPILER}
            ./configure --prefix=${slang_stage_dir}
        BUILD_COMMAND
            ${CMAKE_COMMAND}
            -DSLANG_SOURCE_DIR=${slang_source_dir}
            -DSLANG_MAKE_PROGRAM=${slang_make_program}
            -P ${PROJECT_SOURCE_DIR}/cmake/slang/BuildSlangUnix.cmake
        INSTALL_COMMAND "${slang_make_program}" install
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS "${slang_stage_library}" "${slang_stage_include_dir}/slang.h"
    )

    add_library(slang_dependency_fetched SHARED IMPORTED GLOBAL)
    set_target_properties(
        slang_dependency_fetched
        PROPERTIES
            IMPORTED_LOCATION "${slang_stage_library}"
            INTERFACE_INCLUDE_DIRECTORIES "${slang_stage_include_dir}"
    )
    _set_slang_runtime_properties(
        slang_dependency_fetched
        "${slang_stage_library}"
        "${CMAKE_INSTALL_LIBDIR}"
        "${slang_stage_lib_dir}"
        "${slang_install_rpath}"
    )
    add_dependencies(slang_dependency_fetched slang_external)

    set(${out_target} slang_dependency_fetched PARENT_SCOPE)
    set(${out_include_dir} "${slang_stage_include_dir}" PARENT_SCOPE)
    set(${out_runtime_file} "${slang_stage_library}" PARENT_SCOPE)
    set(${out_runtime_destination} "${CMAKE_INSTALL_LIBDIR}" PARENT_SCOPE)
    set(${out_build_rpath} "${slang_stage_lib_dir}" PARENT_SCOPE)
    set(${out_install_rpath} "${slang_install_rpath}" PARENT_SCOPE)
endfunction()

function(resolve_slang_dependency out_target out_include_dir out_runtime_file out_runtime_destination out_build_rpath out_install_rpath)
    _resolve_slang_from_pkg_config(
        slang_resolved_target
        slang_include_dir
        slang_runtime_file
        slang_runtime_destination
        slang_build_rpath
        slang_install_rpath
    )
    if(slang_resolved_target)
        set(${out_target} "${slang_resolved_target}" PARENT_SCOPE)
        set(${out_include_dir} "${slang_include_dir}" PARENT_SCOPE)
        set(${out_runtime_file} "${slang_runtime_file}" PARENT_SCOPE)
        set(${out_runtime_destination} "${slang_runtime_destination}" PARENT_SCOPE)
        set(${out_build_rpath} "${slang_build_rpath}" PARENT_SCOPE)
        set(${out_install_rpath} "${slang_install_rpath}" PARENT_SCOPE)
        return()
    endif()

    _resolve_slang_from_paths(
        slang_resolved_target
        slang_include_dir
        slang_runtime_file
        slang_runtime_destination
        slang_build_rpath
        slang_install_rpath
    )
    if(slang_resolved_target)
        set(${out_target} "${slang_resolved_target}" PARENT_SCOPE)
        set(${out_include_dir} "${slang_include_dir}" PARENT_SCOPE)
        set(${out_runtime_file} "${slang_runtime_file}" PARENT_SCOPE)
        set(${out_runtime_destination} "${slang_runtime_destination}" PARENT_SCOPE)
        set(${out_build_rpath} "${slang_build_rpath}" PARENT_SCOPE)
        set(${out_install_rpath} "${slang_install_rpath}" PARENT_SCOPE)
        return()
    endif()

    if(NOT MAPSLANG_FETCH_SLANG)
        if(PROJECT_TARGETS_WINDOWS AND NOT PROJECT_TARGETS_MINGW)
            message(FATAL_ERROR
                "mapslang: S-Lang was not found.\n"
                "This toolchain supports system S-Lang discovery only.\n"
                "Provide a compatible S-Lang installation or configure with -DMAPSLANG_BUILD=OFF."
            )
        endif()

        message(FATAL_ERROR
            "mapslang: S-Lang was not found. Install it or enable -DMAPSLANG_FETCH_SLANG=ON."
        )
    endif()

    if(PROJECT_TARGETS_WINDOWS)
        if(NOT PROJECT_TARGETS_MINGW)
            message(FATAL_ERROR
                "mapslang: bundled S-Lang fallback is not implemented for this Windows toolchain.\n"
                "Use a MinGW toolchain, provide a system S-Lang installation, or configure with -DMAPSLANG_BUILD=OFF."
            )
        endif()

        _configure_slang_mingw_fallback(
            slang_resolved_target
            slang_include_dir
            slang_runtime_file
            slang_runtime_destination
            slang_build_rpath
            slang_install_rpath
        )
        set(${out_target} "${slang_resolved_target}" PARENT_SCOPE)
        set(${out_include_dir} "${slang_include_dir}" PARENT_SCOPE)
        set(${out_runtime_file} "${slang_runtime_file}" PARENT_SCOPE)
        set(${out_runtime_destination} "${slang_runtime_destination}" PARENT_SCOPE)
        set(${out_build_rpath} "${slang_build_rpath}" PARENT_SCOPE)
        set(${out_install_rpath} "${slang_install_rpath}" PARENT_SCOPE)
        return()
    endif()

    if(UNIX)
        _configure_slang_unix_fallback(
            slang_resolved_target
            slang_include_dir
            slang_runtime_file
            slang_runtime_destination
            slang_build_rpath
            slang_install_rpath
        )
        set(${out_target} "${slang_resolved_target}" PARENT_SCOPE)
        set(${out_include_dir} "${slang_include_dir}" PARENT_SCOPE)
        set(${out_runtime_file} "${slang_runtime_file}" PARENT_SCOPE)
        set(${out_runtime_destination} "${slang_runtime_destination}" PARENT_SCOPE)
        set(${out_build_rpath} "${slang_build_rpath}" PARENT_SCOPE)
        set(${out_install_rpath} "${slang_install_rpath}" PARENT_SCOPE)
        return()
    endif()

    message(FATAL_ERROR
        "mapslang: bundled S-Lang fallback is not implemented for platform '${CMAKE_SYSTEM_NAME}'."
    )
endfunction()
