function(add_example example_name)
    set(example_dir "${CMAKE_CURRENT_SOURCE_DIR}/examples/${example_name}")

    file(GLOB example_sources CONFIGURE_DEPENDS "${example_dir}/*.c" "${example_dir}/*.h")

    if(PROJECT_TARGETS_WINDOWS)
        file(GLOB example_resources CONFIGURE_DEPENDS "${example_dir}/*.rc")
        list(APPEND example_sources ${example_resources})
    endif()

    add_executable(${example_name} ${example_sources})
    target_link_libraries(${example_name} PRIVATE libadikted::adikted)
    target_include_directories(${example_name} PRIVATE "${example_dir}")
    target_compile_definitions(${example_name} PRIVATE ENTRY_CONFIG_USE_SDL)

    find_package(SDL REQUIRED)
    if(TARGET SDL::SDL)
        target_link_libraries(${example_name} PRIVATE SDL::SDL)
    else()
        target_include_directories(${example_name} PRIVATE ${SDL_INCLUDE_DIR})
        target_link_libraries(${example_name} PRIVATE ${SDL_LIBRARY})
    endif()

    if(UNIX AND NOT APPLE)
        find_package(X11 QUIET)
        if(X11_FOUND)
            target_link_libraries(${example_name} PRIVATE X11::X11)
        endif()
    endif()

    if(BUILD_TESTING)
        add_test(NAME ${example_name} COMMAND ${example_name})
    endif()

    set_target_properties(${example_name} PROPERTIES FOLDER "examples")
endfunction()
