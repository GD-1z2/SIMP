cmake_minimum_required(VERSION 3.16)
project(simp)
set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cglm REQUIRED)
if (LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    include_directories(${GTK3_INCLUDE_DIRS})
endif ()

include_directories(
        include
        ${CGLM_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW3_INCLUDE_DIR}
)

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(simp ${SRC_FILES} src/osdialog/osdialog.c)

if (WIN32)
    target_sources(simp PRIVATE src/osdialog/osdialog_win.c)
else ()
    target_sources(simp PRIVATE src/osdialog/osdialog_gtk3.c)
endif ()

target_link_libraries(simp ${OPENGL_LIBRARIES} ${GLFW3_LIBRARY} ${CGLM_LIBRARY} ${CMAKE_DL_LIBS} -lm)

if (NOT WIN32)
    target_link_libraries(simp -ldl -lGL -lGLU -lX11)
    target_link_libraries(simp ${GTK3_LIBRARIES})
endif ()
