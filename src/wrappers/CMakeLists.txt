cmake_minimum_required(VERSION 3.0)
project(wrappers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Werror -Wall -Wextra -Wpedantic --std=c++17 -fPIC -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Werror -Wall -Wextra -Wpedantic --std=c99 -fPIC -g")

set(SRC
    ${CMAKE_CURRENT_LIST_DIR}/glfw_window.cpp
)

add_library(wrappers ${SRC})

target_link_libraries(wrappers
    -static-libstdc++
    -static-libgcc
    -lglfw
    -lpthread
)