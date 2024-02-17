cmake_minimum_required(VERSION 3.5)

project(gesture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(sdbus-c++)
find_package(LogLibrary)

add_executable(gesture src/main.cpp
                       src/touchhandler.cpp)

target_include_directories(gesture PRIVATE include)

target_link_libraries(gesture PRIVATE SDBusCpp::sdbus-c++ LogLibrary)

install(TARGETS gesture RUNTIME DESTINATION /usr/bin)
