include("${PROJECT_SOURCE_DIR}/cmake/SlangDependency.cmake")

set(mapslang_sources
    input_kb.c
    main.c
    output_scr.c
    scr_actn.c
    scr_clm.c
    scr_cube.c
    scr_help.c
    scr_list.c
    scr_rwrk.c
    scr_slab.c
    scr_thing.c
    scr_tileset.c
    scr_txted.c
    scr_txtgen.c
    textmenu.c
    var_utils.c
)

if(PROJECT_TARGETS_WINDOWS)
    list(APPEND mapslang_sources map_private.rc)
endif()

resolve_slang_dependency(
    mapslang_slang_dependency
    mapslang_slang_include_dir
    mapslang_slang_runtime_file
    mapslang_slang_runtime_destination
    mapslang_slang_build_rpath
    mapslang_slang_install_rpath
)

add_executable(map ${mapslang_sources})

target_compile_features(map PRIVATE c_std_99)
target_compile_definitions(
    map
    PRIVATE
        INSTALL_DATADIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/map"
        $<$<CONFIG:Debug>:MAP_DEBUG=0>
)
target_compile_options(
    map
    PRIVATE
        $<$<C_COMPILER_ID:GNU,Clang>:-Wno-discarded-qualifiers>
        $<$<C_COMPILER_ID:GNU,Clang>:-Wno-unused-parameter>
        $<$<C_COMPILER_ID:GNU,Clang>:-Wno-unused-result>
)
target_include_directories(map PRIVATE "${PROJECT_SOURCE_DIR}")
target_link_libraries(map PRIVATE libadikted::adikted ${mapslang_slang_dependency})

if(mapslang_slang_include_dir)
    target_include_directories(map PRIVATE "${mapslang_slang_include_dir}")
endif()

if(MSVC)
    target_compile_definitions(map PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_link_libraries(map PRIVATE m)
endif()

set_target_properties(map PROPERTIES FOLDER "mapslang")

if(UNIX AND mapslang_slang_build_rpath)
    set_property(TARGET map APPEND PROPERTY BUILD_RPATH "${mapslang_slang_build_rpath}")
endif()

if(UNIX AND mapslang_slang_install_rpath)
    set_property(TARGET map APPEND PROPERTY INSTALL_RPATH "${mapslang_slang_install_rpath}")
endif()

install(
    TARGETS map
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT binaries
)

install(
    FILES map.hlp map.ini map.ico
    DESTINATION ${CMAKE_INSTALL_DATADIR}/map
    COMPONENT resources
)

if(mapslang_slang_runtime_file AND mapslang_slang_runtime_destination)
    set(mapslang_slang_component libraries)
    if("${mapslang_slang_runtime_destination}" STREQUAL "${CMAKE_INSTALL_BINDIR}")
        set(mapslang_slang_component binaries)
    endif()

    install(
        FILES "${mapslang_slang_runtime_file}"
        DESTINATION "${mapslang_slang_runtime_destination}"
        COMPONENT ${mapslang_slang_component}
    )
endif()
