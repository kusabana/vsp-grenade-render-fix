cmake_minimum_required(VERSION 3.5)

project(vsp-grenade-render-fix)

set (CMAKE_CXX_FLAGS "-m32")
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    vsp_utils
    GIT_REPOSITORY https://github.com/kusabana/vsp-utils.git
    GIT_TAG 33d7e9a3ef0aa1ad73bf3248745d08a397a42c69
)

FetchContent_MakeAvailable(vsp_utils)
add_library(vsp-grenade-render-fix SHARED src/plugin.cpp)
target_link_libraries(vsp-grenade-render-fix PRIVATE vsp-utils)
target_include_directories(vsp-grenade-render-fix PUBLIC ${PROJECT_SOURCE_DIR}/include)

