cmake_minimum_required(VERSION 3.1.0)
project(vvvcfg_test)

enable_testing()

set (CMAKE_CXX_STANDARD 14)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} .)

file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE HDRS *.h *.hpp)
add_executable(vvvcfg_test ${SRCS} ${HDRS})
target_link_libraries(vvvcfg_test vvvcfg ${GTEST_LIBRARIES} Threads::Threads)

add_test(vvvcfg_test vvvcfg_test)
