add_library(vorolibrary STATIC IMPORTED)
set_target_properties(vorolibrary PROPERTIES IMPORTED_LOCATION ${se_api_SOURCE_DIR}/voro++-0.4.6/src/libvoro++.a)
include_directories(${se_api_SOURCE_DIR}/voro++-0.4.6/src)
set (CMAKE_CXX_FLAGS "-O3")
file (GLOB _sources *.c*)
add_executable(se_api ${_sources})
target_link_libraries(se_api vorolibrary)
target_link_libraries (se_api boost_program_options)

