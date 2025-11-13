cmake_minimum_required(VERSION 3.10)
project(Engine)

# Build the engine as a static library
add_library(engine STATIC
    enginem.cpp
    render/texture.cpp
    render/renderm.cpp
    tile/tilemap.cpp
    obj/obj_mgr.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(engine
    PUBLIC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${JSONCPP_LIBRARY}
)
