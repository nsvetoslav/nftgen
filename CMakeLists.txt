cmake_minimum_required(VERSION 3.18)
project(nftgen VERSION 1.0.0)

# Set output directories
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

file(GLOB SOURCE_FILES "src/**/*.cpp" "src/**/*.hpp")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC OR WIN32)
    set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")  
endif()

find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

if(MSVC OR WIN32)
    add_compile_options(/ZI)  
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(nftgen "main.cpp" ${SOURCE_FILES} "src/workflow/settings.cpp")

target_link_libraries(nftgen ${OpenCV_LIBS})
