cmake_minimum_required(VERSION 3.10)
project(stream_handle VERSION 0.1.0)


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "*.h")


add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${HEADERS})



target_link_libraries(${PROJECT_NAME} PUBLIC  project_options)


