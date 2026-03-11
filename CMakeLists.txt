set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

cmake_minimum_required(VERSION 4.1)
project(EdusScrapperLibrary)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(CMAKE_CXX_STANDARD 20)

add_library(EdusScrapperLibrary SHARED
        include/Backend.h
        include/library.h
        src/Backend.cpp
        src/scrapper.cpp
        include/scrapper.h)

message(STATUS "Qt6 Found: ${Qt6_FOUND}")
message(STATUS "Qt6 Core Include Dir: ${Qt6Core_INCLUDE_DIRS}")

add_executable(Test1 Tests/Test1.cpp)

target_link_libraries(EdusScrapperLibrary PUBLIC Qt6::Core Qt6::Network)
target_link_libraries(Test1 PRIVATE EdusScrapperLibrary)

target_compile_definitions(EdusScrapperLibrary PRIVATE MYLIBRARY_LIBRARY)

target_include_directories(EdusScrapperLibrary PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
