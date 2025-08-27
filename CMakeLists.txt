cmake_minimum_required(VERSION 3.20)
project(DancoreEngine VERSION 1.0.0 LANGUAGES CXX)

# C++23 стандарт
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции рендеринга
option(DCE_RENDER_VULKAN "Enable Vulkan renderer" ON)
option(DCE_RENDER_DX12 "Enable DirectX 12 renderer" OFF)
option(DCE_RENDER_GL "Enable OpenGL renderer" OFF)

# Включаем DX12 только на Windows
if(WIN32)
    set(DCE_RENDER_DX12 ON CACHE BOOL "Enable DirectX 12 renderer" FORCE)
endif()

# Найти зависимости
find_package(Vulkan REQUIRED)

# SDL3 - используем системную версию из Homebrew
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)

# Создаем алиас для совместимости
if(NOT TARGET SDL3::SDL3)
    add_library(SDL3::SDL3 ALIAS PkgConfig::SDL3)
endif()

# Dear ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui")
if(EXISTS "${IMGUI_DIR}")
    add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )
    
    target_include_directories(imgui PUBLIC 
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )
    
    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui PUBLIC SDL3::SDL3 OpenGL::GL)
    
    if(WIN32)
        target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
    endif()
else()
    message(FATAL_ERROR "Dear ImGui не найден в libs/imgui. Запустите scripts/setup_libs.sh")
endif()

# nlohmann/json
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann_json/json.hpp")
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann_json
    )
else()
    message(FATAL_ERROR "nlohmann/json не найден. Запустите scripts/setup_libs.sh")
endif()

# Создаем статическую библиотеку движка
add_library(dancore STATIC
    # WindowManager
    engine/interfaces/WindowManager/code/WindowManager.cpp
    
    # Рендереры
    engine/render/vulkan/VulkanRenderer.cpp
    engine/render/dx12/DX12Renderer.cpp
    engine/render/opengl/GLRenderer.cpp
    
    # Окна (будут добавлены позже)
    engine/interfaces/windows/MainWindow/code/MainWindow.cpp
    engine/interfaces/windows/EditorWindow/code/EditorWindow.cpp
    engine/interfaces/windows/ViewportWindow/code/ViewportWindow.cpp
)

target_include_directories(dancore PUBLIC
    engine
    engine/interfaces
    engine/render
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann_json
)

target_link_libraries(dancore PUBLIC
    SDL3::SDL3
    imgui
    nlohmann_json
    OpenGL::GL
)

if(DCE_RENDER_VULKAN)
    target_compile_definitions(dancore PUBLIC DCE_VULKAN_ENABLED)
    target_link_libraries(dancore PUBLIC Vulkan::Vulkan)
endif()

if(DCE_RENDER_DX12 AND WIN32)
    target_compile_definitions(dancore PUBLIC DCE_DX12_ENABLED)
    target_link_libraries(dancore PUBLIC d3d12 dxgi)
endif()

if(DCE_RENDER_GL)
    target_compile_definitions(dancore PUBLIC DCE_OPENGL_ENABLED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(dancore PUBLIC OpenGL::GL)
endif()

# Исполняемый файл
add_executable(dancore_app
    main_files/main.cpp
)

target_link_libraries(dancore_app PRIVATE dancore)

# Настройки компилятора
if(MSVC)
    target_compile_options(dancore PRIVATE /W4)
    target_compile_options(dancore_app PRIVATE /W4)
else()
    target_compile_options(dancore PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(dancore_app PRIVATE -Wall -Wextra -pedantic)
endif()

# Копируем конфиги в build директорию
configure_file(configs/startup.json ${CMAKE_BINARY_DIR}/configs/startup.json COPYONLY)
configure_file(configs/windows.json ${CMAKE_BINARY_DIR}/configs/windows.json COPYONLY)
