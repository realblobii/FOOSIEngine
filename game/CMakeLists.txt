cmake_minimum_required(VERSION 3.10)
project(Game)

# Step 1: Create the build folder inside game/
set(GAME_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
file(MAKE_DIRECTORY ${GAME_BUILD_DIR})

# Step 2: Add the executable
add_executable(game main.cpp)

# Step 3: Include engine headers
target_include_directories(game
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..  # engine headers
)

# Step 4: Link engine library
target_link_libraries(game
    PRIVATE
        engine
        jsoncpp
)


# Step 5: Put the binary in game/build/
set_target_properties(game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${GAME_BUILD_DIR}
)
