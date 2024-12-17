cmake_minimum_required(VERSION 3.18)
project(nftgen VERSION 1.0.0)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

file(GLOB_RECURSE SOURCE_FILES "src/**/*.cpp" "src/**/*.hpp")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC OR WIN32)
    set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")  
endif()

find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED COMPONENTS functional) 

if(MSVC OR WIN32)
    add_compile_options(/ZI)  
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

# Custom command to run clang-format
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    message(STATUS "Found clang-format at: ${CLANG_FORMAT}")
    add_custom_target(format ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Formatting files..."
        COMMAND ${CMAKE_COMMAND} -E foreach file IN LISTS SOURCE_FILES
            COMMAND ${CLANG_FORMAT} -i -style=file ${file}
            COMMAND ${CMAKE_COMMAND} -E echo "Formatted: ${file}"
        ENDFOREACH
        COMMENT "Running clang-format on source files"
    )
else()
    message(WARNING "clang-format not found. Formatting will not be applied.")
endif()

add_executable(nftgen "main.cpp" ${SOURCE_FILES} "src/workflow/settings.cpp" "src/workflow/stringutilities.cpp")

target_link_libraries(nftgen ${OpenCV_LIBS})
target_link_libraries(nftgen ${Boost_LIBRARIES})
