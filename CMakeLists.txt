cmake_minimum_required(VERSION 3.15)
project(RaceLogic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_UI "Build UI version (requires display)" ON)

# Dear ImGui - fetch from GitHub
include(FetchContent)

if(BUILD_UI)
    # GLFW
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    # Dear ImGui - we'll add it as source files
    set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
    file(MAKE_DIRECTORY ${IMGUI_DIR})

    # Download Dear ImGui if not present
    if(NOT EXISTS ${IMGUI_DIR}/imgui.h)
        message(STATUS "Downloading Dear ImGui...")
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imgui.h
            ${IMGUI_DIR}/imgui.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imgui.cpp
            ${IMGUI_DIR}/imgui.cpp)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_draw.cpp)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imgui_tables.cpp
            ${IMGUI_DIR}/imgui_tables.cpp)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imgui_widgets.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imgui_internal.h
            ${IMGUI_DIR}/imgui_internal.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imconfig.h
            ${IMGUI_DIR}/imconfig.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imstb_rectpack.h
            ${IMGUI_DIR}/imstb_rectpack.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imstb_textedit.h
            ${IMGUI_DIR}/imstb_textedit.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/imstb_truetype.h
            ${IMGUI_DIR}/imstb_truetype.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/backends/imgui_impl_glfw.h
            ${IMGUI_DIR}/imgui_impl_glfw.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/backends/imgui_impl_glfw.cpp
            ${IMGUI_DIR}/imgui_impl_glfw.cpp)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/backends/imgui_impl_opengl3.h
            ${IMGUI_DIR}/imgui_impl_opengl3.h)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/backends/imgui_impl_opengl3.cpp
            ${IMGUI_DIR}/imgui_impl_opengl3.cpp)
        file(DOWNLOAD
            https://raw.githubusercontent.com/ocornut/imgui/v1.89.9/backends/imgui_impl_opengl3_loader.h
            ${IMGUI_DIR}/imgui_impl_opengl3_loader.h)
    endif()

    # ImGui library
    add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC ${IMGUI_DIR})
    target_link_libraries(imgui PUBLIC glfw)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE MODELS_SOURCES "src/models/*.cpp")
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE UI_SOURCES "src/ui/*.cpp")

# Test executable (core logic only, no UI)
add_executable(RaceLogicTest
    src/test_core.cpp
    ${MODELS_SOURCES}
    ${CORE_SOURCES}
)

# Main executable with UI
if(BUILD_UI)
    add_executable(RaceLogic
        src/main.cpp
        ${MODELS_SOURCES}
        ${CORE_SOURCES}
        ${UI_SOURCES}
    )

    target_link_libraries(RaceLogic PRIVATE imgui glfw)

    # Platform-specific libraries
    if(UNIX AND NOT APPLE)
        target_link_libraries(RaceLogic PRIVATE GL dl pthread)
    elseif(APPLE)
        target_link_libraries(RaceLogic PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    elseif(WIN32)
        target_link_libraries(RaceLogic PRIVATE opengl32)
    endif()
endif()
