cmake_minimum_required(VERSION 3.14)
project(dead_drop_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CPM Setup
set(CPM_DOWNLOAD_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM_0.40.0.cmake")
set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/contrib")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.0/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Asio (Crow dependency)
CPMAddPackage(
    NAME asio
    GITHUB_REPOSITORY chriskohlhoff/asio
    GIT_TAG asio-1-30-2
    OPTIONS "ASIO_NO_DEPRECATED"
)

if(asio_ADDED)
    add_library(asio::asio INTERFACE IMPORTED)
    target_include_directories(asio::asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
    target_compile_definitions(asio::asio INTERFACE ASIO_STANDALONE)
endif()

# Crow (Web Framework)
# Crow defaults to using standalone asio if available or finding package
CPMAddPackage(
    NAME Crow
    GITHUB_REPOSITORY CrowCpp/Crow
    GIT_TAG v1.2.0
    OPTIONS "CROW_BUILD_EXAMPLES OFF" "CROW_BUILD_TESTS OFF"
)

# nlohmann_json (JSON)
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    GIT_TAG v3.11.3
)

add_executable(dead_drop_service src/main.cpp)

target_link_libraries(dead_drop_service PRIVATE Crow::Crow nlohmann_json::nlohmann_json)
target_include_directories(dead_drop_service PRIVATE ${asio_SOURCE_DIR}/asio/include)

# Copy templates to build directory
add_custom_command(TARGET dead_drop_service POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/templates
    $<TARGET_FILE_DIR:dead_drop_service>/templates
)
