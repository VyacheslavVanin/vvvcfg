cmake_minimum_required(VERSION 3.1)
project(vvvcfg)
set (CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)

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

# Installation
set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
set_target_properties(vvvcfg PROPERTIES PUBLIC_HEADER
    "vvvcfg/vvvcfg.hpp;vvvcfg/cfg_node.hpp")
INSTALL(TARGETS vvvcfg vvvcfg_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/vvvcfg
)
