cmake_minimum_required(VERSION 3.5)

project(gesture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(sdbus-c++)

add_executable(gesture src/main.cpp
                       src/touchhandler.cpp
                       src/logger.cpp)

target_include_directories(gesture PRIVATE include)

target_link_libraries(gesture PRIVATE SDBusCpp::sdbus-c++)

install(TARGETS gesture RUNTIME DESTINATION /usr/bin)
