include(CMakePackageConfigHelpers)

option(USE_RNG_MT "Use Mersenne Twister RNG." ON)

set(adikted_public_headers
    adikted.h
    arr_utils.h
    bulcommn.h
    dernc.h
    draw_map.h
    globals.h
    graffiti.h
    lbfileio.h
    lev_column.h
    lev_data.h
    lev_files.h
    lev_script.h
    lev_things.h
    memfile.h
    msg_log.h
    obj_actnpts.h
    obj_column.h
    obj_column_def.h
    obj_cube.h
    obj_slabs.h
    obj_things.h
    rng.h
    xcubtxtr.h
    xtabdat8.h
    xtabjty.h
)

set(adikted_sources
    arr_utils.c
    bulcommn.c
    dernc.c
    draw_map.c
    graffiti.c
    graffiti_font.c
    lbfileio.c
    lev_column.c
    lev_data.c
    lev_files.c
    lev_script.c
    lev_things.c
    libadi_main.c
    memfile.c
    msg_log.c
    obj_actnpts.c
    obj_column.c
    obj_column_def.c
    obj_column_per.c
    obj_slabs.c
    obj_things.c
    rng.c
    xcubtxtr.c
    xtabdat8.c
    xtabjty.c
)

if(USE_RNG_MT)
    list(APPEND adikted_sources mtwist/mtwist.c)
endif()

add_library(adikted ${adikted_sources})
add_library(libadikted::adikted ALIAS adikted)

if(PROJECT_TARGETS_WINDOWS)
    target_sources(adikted PRIVATE adikted_private.rc)
endif()

target_compile_features(adikted PUBLIC c_std_99)
target_include_directories(
    adikted
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(
    adikted
    PUBLIC
        $<$<BOOL:${PROJECT_TARGETS_WINDOWS}>:PROJECT_TARGETS_WINDOWS>
    PRIVATE
        $<$<NOT:$<BOOL:${PROJECT_TARGETS_WINDOWS}>>:_POSIX_C_SOURCE=200809L>
)

if(USE_RNG_MT)
    target_compile_definitions(adikted PRIVATE RNG_MT)
endif()

if(PROJECT_TARGETS_WINDOWS AND BUILD_SHARED_LIBS)
    target_compile_definitions(adikted PRIVATE BUILD_DLL USE_FASTCALL)
endif()

if(MSVC)
    target_compile_definitions(adikted PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_link_libraries(adikted PUBLIC m)
endif()

set_target_properties(
    adikted
    PROPERTIES
        EXPORT_NAME adikted
        OUTPUT_NAME adikted
        DEBUG_POSTFIX _d
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(target_name "adikted")
string(JOIN " " stdclibs ${CMAKE_C_STANDARD_LIBRARIES})
set(install_prefix "${CMAKE_INSTALL_PREFIX}")
set(install_libdir "${CMAKE_INSTALL_LIBDIR}")
set(install_includedir "${CMAKE_INSTALL_INCLUDEDIR}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/libadikted.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libadikted.pc"
    @ONLY
)

install(
    TARGETS adikted
    EXPORT libadiktedTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT binaries
)

install(FILES ${adikted_public_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libadikted COMPONENT headers)

if(USE_RNG_MT)
    install(FILES mtwist/mtwist.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libadikted/mtwist COMPONENT headers)
endif()

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/libadikted.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT development
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libadiktedConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/libadiktedConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libadiktedConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libadikted"
)

install(
    EXPORT libadiktedTargets
    FILE libadiktedTargets.cmake
    NAMESPACE libadikted::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libadikted
    COMPONENT development
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libadiktedConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libadiktedConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libadikted
    COMPONENT development
)
