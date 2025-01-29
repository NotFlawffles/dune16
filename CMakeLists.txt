cmake_minimum_required(VERSION 3.10)

set(NAME dune16)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -ggdb")

project(${NAME})

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.c")

link_libraries(dync)

add_executable(${NAME} ${SOURCES})
