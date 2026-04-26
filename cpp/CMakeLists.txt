cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(RocketStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

if(TARGET glfw)
    set(ROCKETSTUDIO_GLFW_TARGET glfw)
elseif(TARGET glfw3)
    set(ROCKETSTUDIO_GLFW_TARGET glfw3)
else()
    message(FATAL_ERROR "glfw3 package was found, but no glfw CMake target is available")
endif()

FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip
)

FetchContent_Declare(
    implot
    URL https://github.com/epezent/implot/archive/refs/tags/v0.16.zip
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

FetchContent_GetProperties(implot)
if(NOT implot_POPULATED)
    FetchContent_Populate(implot)
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC ${ROCKETSTUDIO_GLFW_TARGET} OpenGL::GL)

add_library(implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

target_include_directories(implot PUBLIC
    ${implot_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
)

target_link_libraries(implot PUBLIC imgui)

add_executable(RocketStudio
    main.cpp
    app/App.cpp
    app/Gui.cpp
    app/Renderer2D.cpp
    app/Renderer3D.cpp
    app/Theme.cpp
    physics/Rocket.cpp
    physics/Environment.cpp
    physics/PhysicsEngine.cpp
    physics/Integrator.cpp
    physics/SimulationRecorder.cpp
    io/CSVWriter.cpp
    io/JSONWriter.cpp
    io/ConfigIO.cpp
    utils/Logger.cpp
    utils/Timer.cpp
)

target_include_directories(RocketStudio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RocketStudio PRIVATE
    imgui
    implot
    ${ROCKETSTUDIO_GLFW_TARGET}
    OpenGL::GL
)

if(MINGW)
    target_link_libraries(RocketStudio PRIVATE opengl32 gdi32 imm32 shell32 dwmapi gdiplus)
endif()

add_custom_command(TARGET RocketStudio POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../data"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../assets/fonts"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../assets/icons"
)
