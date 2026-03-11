cmake_minimum_required(VERSION 4.1)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/ExportedPackage" CACHE PATH "Installation Prefix" FORCE)
project(EdusScrapperLibrary)


set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)


find_package(Qt6 REQUIRED COMPONENTS Core Network)


add_library(EdusScrapperLibrary SHARED
        include/Backend.h
        include/library.h
        include/scrapper.h
        src/Backend.cpp
        src/scrapper.cpp
)


target_include_directories(EdusScrapperLibrary PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_definitions(EdusScrapperLibrary PRIVATE MYLIBRARY_LIBRARY)
target_link_libraries(EdusScrapperLibrary PUBLIC Qt6::Core Qt6::Network)


add_executable(Test1 tests_src/Test1.cpp)

target_link_libraries(Test1 PRIVATE EdusScrapperLibrary Qt6::Core Qt6::Network)


install(TARGETS EdusScrapperLibrary
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
)

install(TARGETS Test1
        RUNTIME DESTINATION bin    # Move this to 'bin' so it's next to the .dll!
)

install(DIRECTORY include/ DESTINATION include)