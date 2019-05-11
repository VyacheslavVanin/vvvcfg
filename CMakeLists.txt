cmake_minimum_required(VERSION 3.1)
project(vvvcfg)

set (CMAKE_CXX_STANDARD 14)

# Set include path
include_directories(.)

# Set Source files
file(GLOB_RECURSE SRCS token_parser/*.cpp token_parser/*.c
                       vvvcfg/*.cpp vvvcfg/*.c)
file(GLOB_RECURSE HDRS *.h *.hpp)

add_library(vvvcfg SHARED ${SRCS} ${HDRS})
add_library(vvvcfg_static STATIC ${SRCS} ${HDRS})
set_target_properties(vvvcfg_static PROPERTIES OUTPUT_NAME vvvcfg)

target_link_libraries(vvvcfg)


option(WITH_TESTS "BUILD WITH TESTS" OFF)
if(WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(WITH_TESTS)

# Installation
set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
set_target_properties(vvvcfg PROPERTIES PUBLIC_HEADER
    "vvvcfg/vvvcfg.hpp;vvvcfg/cfg_node.hpp;vvvcfg/cfg_node_value.hpp;vvvcfg/any.hpp")
INSTALL(TARGETS vvvcfg vvvcfg_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/vvvcfg
)
