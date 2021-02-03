cmake_minimum_required(VERSION 3.0)
project(build_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Werror -Wall -Wextra -Wpedantic --std=c++17 -fPIC -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Werror -Wall -Wextra -Wpedantic --std=c99 -fPIC -g")

set(SRC
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

include_directories("${CMAKE_SOURCE_DIR}/../wrappers/")

add_executable(build_test ${SRC})

target_link_libraries(build_test
    -static-libstdc++
    -static-libgcc
    "${CMAKE_SOURCE_DIR}/../wrappers/build/libwrappers.a"
    -lglfw
    -lvulkan
    -ldl
    -lpthread
    -lX11
    -lXxf86vm
    -lXrandr
    -lXi
)