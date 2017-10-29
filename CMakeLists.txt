cmake_minimum_required(VERSION 3.1)
project(vvvcfg)
set (CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)

# Set include path
include_directories(.)

# Set Source files
file(GLOB_RECURSE SRCS token_parser/*.cpp token_parser/*.c
                       node_parser/*.cpp node_parser/*.c)
file(GLOB_RECURSE HDRS *.h *.hpp)
add_library(vvvcfg SHARED ${SRCS} ${HDRS})

target_link_libraries(vvvcfg)

# Installation
set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
set_target_properties(vvvcfg PROPERTIES PUBLIC_HEADER
    "node_parser/node_parser.hpp;node_parser/cfg_node.hpp")
INSTALL(TARGETS vvvcfg 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/vvvcfg
)
