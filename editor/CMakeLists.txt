cmake_minimum_required(VERSION 3.19)
project(editor LANGUAGES CXX)

add_executable(editor src/main.cpp)

set_target_properties(editor PROPERTIES OUTPUT_NAME "editor")

target_link_libraries(editor
    PRIVATE
        EngineProject
        assimp::assimp
)
