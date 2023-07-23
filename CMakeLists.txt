cmake_minimum_required(VERSION 3.10)
project(SimpleLabManager)
include_directories(inc lib/AsyncTCP/inc inc/database inc/file inc/network)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(SOURCE_DIR src src/database src/file src/network)
#在所有目录下查找所有源文件
foreach (dir ${SOURCE_DIR})
    aux_source_directory(${dir} SOURCE_FILES)
endforeach ()

#检查外部链接库
if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

#添加可执行文件
add_executable(SimpleLabManager ${SOURCE_FILES})
target_link_libraries(SimpleLabManager PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)