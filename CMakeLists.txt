cmake_minimum_required(VERSION 3.14)
project(SUAV2025)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

set(ASSIMP_INCLUDE_DIR "/opt/homebrew/Cellar/assimp/5.4.3/include")
set(ASSIMP_LIBRARY "/opt/homebrew/Cellar/assimp/5.4.3/lib/libassimp.dylib")

include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
)

FetchContent_MakeAvailable(glm)

include_directories(
    ${CMAKE_SOURCE_DIR}/dep/incl
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/backends
    ${CMAKE_SOURCE_DIR}/dep/incl/glad
    ${CMAKE_SOURCE_DIR}/dep/incl/glfw
    ${CMAKE_SOURCE_DIR}/dep/incl/stb
    ${CMAKE_SOURCE_DIR}/dep/incl/KHR
    ${CMAKE_SOURCE_DIR}/main/incl
    ${ASSIMP_INCLUDE_DIR}
)

# Source files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/main/src/main.cpp
    ${CMAKE_SOURCE_DIR}/main/src/camera.cpp
    ${CMAKE_SOURCE_DIR}/main/src/GUIController.cpp
    ${CMAKE_SOURCE_DIR}/main/src/model.cpp
    ${CMAKE_SOURCE_DIR}/main/src/shader.cpp

    ${CMAKE_SOURCE_DIR}/dep/src/glad/glad.c

    ${CMAKE_SOURCE_DIR}/dep/src/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/dep/src/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/dep/src/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/dep/src/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/dep/src/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/dep/src/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/dep/src/imgui/backends/imgui_impl_opengl3.cpp
)

# Header files
set(HEADERS
    ${CMAKE_SOURCE_DIR}/main/incl/camera.h
    ${CMAKE_SOURCE_DIR}/main/incl/GUIController.h
    ${CMAKE_SOURCE_DIR}/main/incl/model.h
    ${CMAKE_SOURCE_DIR}/main/incl/shader.h

    ${CMAKE_SOURCE_DIR}/dep/incl/KHR/khrplatform.h
    ${CMAKE_SOURCE_DIR}/dep/incl/glfw/glfw3.h
    ${CMAKE_SOURCE_DIR}/dep/incl/glfw/glfw3native.h
    ${CMAKE_SOURCE_DIR}/dep/incl/glad/glad.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/imgui.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/imgui_internal.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/imconfig.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/imstb_rectpack.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/imstb_textedit.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/imstb_truetype.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/backends/imgui_impl_glfw.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/backends/imgui_impl_opengl3.h
    ${CMAKE_SOURCE_DIR}/dep/incl/imgui/backends/imgui_impl_opengl3_loader.h
    ${CMAKE_SOURCE_DIR}/dep/incl/stb/stb_image.h
)

# Shaders
set(SHADERS
    ${CMAKE_SOURCE_DIR}/main/shaders/vertex/main.vs
    ${CMAKE_SOURCE_DIR}/main/shaders/fragment/main.fs
)

# Add executable
add_executable(app ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(app PRIVATE 
    glfw
    glm::glm
    "${ASSIMP_LIBRARY}"
    "-framework OpenGL"
)

# Copy shaders to the build directory
foreach(SHADER ${SHADERS})
    add_custom_command(
        TARGET app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${CMAKE_BINARY_DIR}
    )
endforeach()