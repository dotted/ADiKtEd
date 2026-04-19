if(NOT DEFINED SLANG_SOURCE_DIR)
    message(FATAL_ERROR "SLANG_SOURCE_DIR is required.")
endif()

if(NOT DEFINED SLANG_STAGE_DIR)
    message(FATAL_ERROR "SLANG_STAGE_DIR is required.")
endif()

set(slang_stage_include_dir "${SLANG_STAGE_DIR}/include")
set(slang_stage_lib_dir "${SLANG_STAGE_DIR}/lib")
set(slang_stage_bin_dir "${SLANG_STAGE_DIR}/bin")
set(slang_source_header_dir "${SLANG_SOURCE_DIR}/src")
set(slang_source_object_dir "${SLANG_SOURCE_DIR}/src/gw32objs")

file(MAKE_DIRECTORY "${slang_stage_include_dir}" "${slang_stage_lib_dir}" "${slang_stage_bin_dir}")

file(GLOB slang_headers "${slang_source_header_dir}/*.h")
foreach(slang_header IN LISTS slang_headers)
    file(COPY "${slang_header}" DESTINATION "${slang_stage_include_dir}")
endforeach()

file(COPY "${slang_source_object_dir}/libslang.a" DESTINATION "${slang_stage_lib_dir}")
file(COPY "${slang_source_object_dir}/libslang.dll" DESTINATION "${slang_stage_bin_dir}")
