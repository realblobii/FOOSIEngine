cmake_minimum_required(VERSION 3.10)
project(Engine)

# Build the engine as a static library
add_library(engine STATIC
    enginem.cpp
    render/texture.cpp
    render/renderm.cpp
    render/glAbstract.cpp
    tile/tilemap.cpp
    obj/obj_mgr.cpp
)

# Glad is its own static library
add_library(glad STATIC ../incl/glad/glad.c)
add_library(stb_image STATIC ../incl/stb_image.cpp)

# Glad needs its include dir
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/incl)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/incl)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
)
target_include_directories(glad
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../incl
)
target_include_directories(stb_image
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../incl
)
target_link_libraries(engine
    PUBLIC
        glad               # you forgot this
        stb_image
        OpenGL::GL
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${JSONCPP_LIBRARIES}   # correct plural variable
)
