set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

cmake_minimum_required(VERSION 4.1)
project(EdusScrapperLibrary)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(CMAKE_CXX_STANDARD 20)

add_library(EdusScrapperLibrary SHARED library.cpp
        Backend.h
        library.h
        Backend.cpp
        scrapper.cpp
        scrapper.h)

message(STATUS "Qt6 Found: ${Qt6_FOUND}")
message(STATUS "Qt6 Core Include Dir: ${Qt6Core_INCLUDE_DIRS}")

add_executable(Test1 Tests/Test1.cpp)

target_link_libraries(EdusScrapperLibrary PUBLIC Qt6::Core Qt6::Network)
target_link_libraries(Test1 PRIVATE EdusScrapperLibrary)
