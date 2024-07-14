cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})  # 设置可执行文件输出路径为项目根目录

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(Threads REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

add_executable(parser src/parser.cc)
target_link_libraries(parser Boost::system Boost::filesystem)

add_executable(debug_server src/debug.cc)
target_link_libraries(debug_server jsoncpp_lib)

add_executable(server src/server.cc)
target_link_libraries(server jsoncpp_lib Threads::Threads)