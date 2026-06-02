cmake_minimum_required(VERSION 3.16)
project(MyGame)

set(CMAKE_CXX_STANDARD 23)

# 1. Turn off raylib tests/examples to make your compilation super fast
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

# 2. CHANGE THIS PATH to the exact absolute path of your cloned raylib folder
set(LOCAL_RAYLIB_PATH "/home/nelson/raylib/")

# 3. Add the local raylib directory to your project build tree
add_subdirectory(${LOCAL_RAYLIB_PATH} ${CMAKE_CURRENT_BINARY_DIR}/raylib)

# 4. Define your executable target
add_executable(main main.cpp)

# 5. Link raylib to your 'main' executable target
target_include_directories(main PRIVATE ${LOCAL_RAYLIB_PATH}/include)
target_link_libraries(main PRIVATE raylib)
