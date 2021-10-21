cmake_minimum_required(VERSION 3.16)

project(fractal_viewer)
set(CMAKE_CXX_STANDARD 20)

add_executable(fractal_viewer src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_custom_target(copy-files ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy-files)

target_link_libraries(${PROJECT_NAME} GL glfw GLEW)

add_compile_options(-march=haswell)