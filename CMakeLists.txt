cmake_minimum_required(VERSION 3.4)
project(zaperoids)

set(CMAKE_CXX_STANDARD 14)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
add_definitions(-DLOGGING_ENABLED)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

include_directories(
        ${CMAKE_SOURCE_DIR}/third_party/include
        ${CMAKE_SOURCE_DIR}/third_party/include/zap
        /Users/otgaard/Development/zap/third_party/include)

link_directories(
        ${CMAKE_SOURCE_DIR}/third_party/lib
        /Users/otgaard/Development/zap/third_party/lib)

set(SOURCE_FILES main.cpp vector_font.cpp vector_font.hpp world.cpp world.hpp common.hpp game.hpp)

if(APPLE)
    find_library(COCOA Cocoa REQUIRED)
    find_library(IOKIT IOKit)
    find_library(CARBON Carbon)
    find_library(COREVIDEO CoreVideo)
    find_library(QUARTZ QuartzCore)
    find_library(OPENGL OpenGL)
    set(GLFW_LIBS glfw3 ${COCOA} ${IOKIT} ${CARBON} ${COREVIDEO} ${QUARTZ} ${OPENGL})
    set(LOADER_LIBS ${GLEW_LIB} ${OPENGL})
elseif(UNIX)
    find_library(OPENGL GL)
    find_library(M m)
    find_library(DYNLINK dl)
    find_library(XINERAMA Xinerama)
    find_library(XRANDR Xrandr)
    find_library(XI Xi)
    find_library(XCURSOR Xcursor)
    find_library(X11 X11)
    find_library(XXF86VM Xxf86vm)
    find_library(PTHREAD pthread)
    set(GLFW_LIBS glfw3 ${OPENGL} ${M} ${DYNLINK} ${XINERAMA} ${XRANDR} ${XI} ${XCURSOR} ${X11} ${XXF86VM} ${PTHREAD})
    set(LOADER_LIBS ${GLEW_LIB})
elseif(WIN32)
    set(GLFW_LIBS glfw3.lib opengl32.lib ${GLEW_LIB})
    set(LOADER_LIBS )
endif(APPLE)

add_executable(zaperoids ${SOURCE_FILES})
target_link_libraries(zaperoids zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})
