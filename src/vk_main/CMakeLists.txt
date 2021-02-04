cmake_minimum_required(VERSION 3.0)
project(vk_triangle)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Werror -Wall -Wextra -Wpedantic --std=c++17 -fPIC -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Werror -Wall -Wextra -Wpedantic --std=c99 -fPIC -g")

set(SRC
	${CMAKE_CURRENT_LIST_DIR}/vk_main.cpp
	${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

include_directories("${CMAKE_SOURCE_DIR}/../wrappers/")

add_executable(vk_triangle ${SRC})

target_link_libraries(vk_triangle
	-static-libstdc++
	-static-libgcc
	"${CMAKE_SOURCE_DIR}/../wrappers/build/libwrappers.a"
	-lglfw
	-lvulkan
	-lpthread
)