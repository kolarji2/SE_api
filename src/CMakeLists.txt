cmake_minimum_required(VERSION 3.14)
set (CMAKE_CXX_FLAGS "-O3")
file (GLOB _sources *.c*)
add_executable(se_api ${_sources})
#Voro ++ specify path to the voro library
#include_directories(/usr/local/include/voro++)
include_directories(/usr/include/voro++)
target_link_libraries(se_api /lib/libvoro++.a)
#target_link_libraries(se_api /usr/local/lib/libvoro++.a)
target_link_libraries (se_api boost_program_options)
target_link_libraries (se_api boost_system)
target_link_libraries (se_api boost_filesystem)

install(TARGETS se_api DESTINATION /usr/local/bin)
