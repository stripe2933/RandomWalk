cmake_minimum_required(VERSION 3.25)
project(RandomWalk)

set(CMAKE_CXX_STANDARD 20)

add_executable(RandomWalk main.cpp src/App.cpp include/App.hpp src/opengl_utils.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link dependencies.
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glfw OpenGL::GL glm::glm GLEW::GLEW fmt::fmt)

# Precompile the frequently used header files.
target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        <GL/glew.h>
        <GLFW/glfw3.h>)