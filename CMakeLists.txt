cmake_minimum_required(VERSION 3.15)

project(
    ADiKtEd
    VERSION 0.9.6.1
    DESCRIPTION "Dungeon Keeper 1 map editor"
    LANGUAGES C
)

set(PROJECT_TARGETS_WINDOWS OFF)
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PROJECT_TARGETS_WINDOWS ON)
elseif(CMAKE_C_PLATFORM_ID STREQUAL "MinGW")
    set(PROJECT_TARGETS_WINDOWS ON)
elseif(CMAKE_C_COMPILER MATCHES [[(^|[\\/])mingw(32|64)([\\/])bin([\\/]).*(cc|gcc)([.]exe)?$]])
    set(PROJECT_TARGETS_WINDOWS ON)
endif()

set(PROJECT_TARGETS_MINGW OFF)
if(MINGW OR CMAKE_C_PLATFORM_ID STREQUAL "MinGW")
    set(PROJECT_TARGETS_MINGW ON)
elseif(CMAKE_C_COMPILER MATCHES [[(^|[\\/])mingw(32|64)([\\/])bin([\\/]).*(cc|gcc)([.]exe)?$]])
    set(PROJECT_TARGETS_MINGW ON)
endif()

if(PROJECT_TARGETS_WINDOWS)
    enable_language(RC)
endif()

include(GNUInstallDirs)
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(
        CMAKE_INSTALL_PREFIX
        "${CMAKE_BINARY_DIR}/install"
        CACHE PATH
        "Install path prefix."
        FORCE
    )
endif()

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries by default.")
endif()

if(PROJECT_TARGETS_MINGW)
    set(_mapslang_fetch_slang_default ON)
else()
    set(_mapslang_fetch_slang_default OFF)
endif()

option(MAPSLANG_BUILD "Build the mapslang TUI editor." ON)
option(ADIKTED_BUILD_EXAMPLES "Build ADiKtEd examples." OFF)
option(
    MAPSLANG_FETCH_SLANG
    "Fetch and build S-Lang 2.3.2 when it is not available for mapslang."
    ${_mapslang_fetch_slang_default}
)
set(
    MAPSLANG_SLANG_SOURCE
    "release"
    CACHE STRING
    "Source used when fetching mapslang's S-Lang 2.3.2 dependency (release or git)."
)
set_property(CACHE MAPSLANG_SLANG_SOURCE PROPERTY STRINGS release git)

add_compile_options(
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wextra>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wpedantic>
    $<$<C_COMPILER_ID:GNU,Clang>:-Werror-implicit-function-declaration>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wno-conversion>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wno-sign-compare>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wno-switch>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wno-traditional-conversion>
)

add_subdirectory(libadikted)

if(MAPSLANG_BUILD)
    add_subdirectory(mapslang)
endif()

if(ADIKTED_BUILD_EXAMPLES)
    include(cmake/examples.cmake)

    set(ADIKTED_EXAMPLES putgems puttrain viewmap putemple)
    foreach(example IN LISTS ADIKTED_EXAMPLES)
        add_example("${example}")
    endforeach()

    install(
        TARGETS ${ADIKTED_EXAMPLES}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT binaries
    )
endif()
