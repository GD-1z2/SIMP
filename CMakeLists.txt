cmake_minimum_required(VERSION 3.16)
project(simp)
set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cglm REQUIRED)

include_directories(
        include
        ${CGLM_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW3_INCLUDE_DIR}
)

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(simp ${SRC_FILES})

target_link_libraries(simp ${OPENGL_LIBRARIES} ${GLFW3_LIBRARY} ${CGLM_LIBRARY} ${CMAKE_DL_LIBS} -lm)

if (NOT WIN32)
    target_link_libraries(simp -ldl -lGL -lGLU -lX11)
endif ()
