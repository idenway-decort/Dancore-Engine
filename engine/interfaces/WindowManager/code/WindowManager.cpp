#include "../interface/WindowManager.h"
#include "../../windows/IAppWindow.h"
#include "../../windows/MainWindow/interface/MainWindow.h"
#include "../../windows/EditorWindow/interface/EditorWindow.h"
#include "../../windows/ViewportWindow/interface/ViewportWindow.h"
#include "../../../render/interfaces/IRenderer.h"
#include "../../../render/vulkan/VulkanRenderer.h"
#include "../../../render/dx12/DX12Renderer.h"
#include "../../../render/opengl/GLRenderer.h"

#include <SDL3/SDL.h>
#include <json.hpp>
#include <imgui_impl_sdl3.h>
#include <fstream>
#include <iostream>
#include <cstdlib>

using json = nlohmann::json;

WindowManager::WindowManager() = default;

WindowManager::~WindowManager() {
    cleanup();
}

bool WindowManager::initFromConfigs(const char* startupPath, const char* windowsPath) {
    std::cout << "Инициализация WindowManager..." << std::endl;

    if (!loadStartupConfig(startupPath)) {
        std::cerr << "Ошибка загрузки конфигурации startup" << std::endl;
        return false;
    }

    if (!loadWindowsConfig(windowsPath)) {
        std::cerr << "Ошибка загрузки конфигурации окон" << std::endl;
        return false;
    }

    if (!initSDL()) {
        std::cerr << "Ошибка инициализации SDL" << std::endl;
        return false;
    }

    if (!createWindow()) {
        std::cerr << "Ошибка создания окна" << std::endl;
        return false;
    }

    if (!initRenderer()) {
        std::cerr << "Ошибка инициализации рендерера" << std::endl;
        return false;
    }

    if (!createAppWindows()) {
        std::cerr << "Ошибка создания окон приложения" << std::endl;
        return false;
    }

    std::cout << "WindowManager успешно инициализирован" << std::endl;
    return true;
}

void WindowManager::mainLoop() {
    std::cout << "Запуск главного цикла..." << std::endl;
    m_running = true;

    SDL_Event event;
    while (m_running) {
        // Обработка событий SDL
        while (SDL_PollEvent(&event)) {
            // Передаем события в ImGui
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_running = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    if (event.window.windowID == SDL_GetWindowID(m_sdlWindow)) {
                        m_width = event.window.data1;
                        m_height = event.window.data2;
                        if (m_renderer) {
                            m_renderer->resize(m_width, m_height);
                        }
                    }
                    break;
            }
        }

        // Сначала обновляем все окна (в т.ч. подготовку кадра ImGui внутри одного из окон)
        for (auto& window : m_appWindows) {
            window->update();
        }
        // Затем рисуем все окна в рамках одного кадра ImGui
        for (auto& window : m_appWindows) {
            window->draw();
        }
    }

    std::cout << "Завершение главного цикла" << std::endl;
}

bool WindowManager::loadStartupConfig(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << path << std::endl;
        return false;
    }

    try {
        json config;
        file >> config;

        m_appName = config.value("appName", "Dancore Engine");
        m_width = config.value("width", 1600);
        m_height = config.value("height", 900);
        m_rendererType = config.value("renderer", "Vulkan");
        m_vsync = config.value("vsync", true);

        std::cout << "Загружена конфигурация startup:" << std::endl;
        std::cout << "  App Name: " << m_appName << std::endl;
        std::cout << "  Size: " << m_width << "x" << m_height << std::endl;
        std::cout << "  Renderer: " << m_rendererType << std::endl;
        std::cout << "  VSync: " << (m_vsync ? "true" : "false") << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка парсинга startup.json: " << e.what() << std::endl;
        return false;
    }
}

bool WindowManager::loadWindowsConfig(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << path << std::endl;
        return false;
    }

    try {
        json config;
        file >> config;

        if (config.contains("windows") && config["windows"].is_array()) {
            for (const auto& windowConfig : config["windows"]) {
                if (windowConfig.contains("type")) {
                    std::string type = windowConfig["type"];
                    m_windowTypes.push_back(type);
                    std::cout << "Найден тип окна: " << type << std::endl;
                }
            }
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка парсинга windows.json: " << e.what() << std::endl;
        return false;
    }
}

bool WindowManager::initSDL() {
    setenv("SDL_VIDEODRIVER", "cocoa", 1);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "Ошибка инициализации SDL: " << SDL_GetError() << std::endl;
        int num = SDL_GetNumVideoDrivers();
        std::cerr << "Доступные видео-драйверы (" << num << "):" << std::endl;
        for (int i = 0; i < num; ++i) {
            const char* name = SDL_GetVideoDriver(i);
            std::cerr << "  - " << (name ? name : "<null>") << std::endl;
        }
        return false;
    }
    const char* driver = SDL_GetCurrentVideoDriver();
    std::cout << "SDL успешно инициализирован (video driver: " << (driver ? driver : "<none>") << ")" << std::endl;
    return true;
}

bool WindowManager::createWindow() {
    std::cout << "Настройка OpenGL атрибутов..." << std::endl;
    
    // Настраиваем атрибуты OpenGL Core Profile 3.0+
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Создаем окно с OpenGL флагами
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    std::cout << "Создание SDL окна..." << std::endl;
    m_sdlWindow = SDL_CreateWindow(
        m_appName.c_str(),
        m_width, m_height,
        windowFlags
    );

    if (!m_sdlWindow) {
        std::cerr << "Ошибка создания окна SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Создание OpenGL контекста..." << std::endl;
    // Создаем OpenGL контекст
    m_glContext = SDL_GL_CreateContext(m_sdlWindow);
    if (!m_glContext) {
        std::cerr << "Ошибка создания OpenGL контекста: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_sdlWindow);
        m_sdlWindow = nullptr;
        return false;
    }

    // Включаем VSync
    std::cout << "Настройка VSync..." << std::endl;
    SDL_GL_SetSwapInterval(1);

    std::cout << "OpenGL окно успешно создано: " << m_width << "x" << m_height << std::endl;
    return true;
}

bool WindowManager::initRenderer() {
    RenderInitInfo initInfo;
    initInfo.nativeWindow = m_sdlWindow;
    initInfo.width = m_width;
    initInfo.height = m_height;
    initInfo.appName = m_appName.c_str();

    if (m_rendererType == "Vulkan") {
        m_renderer = std::make_unique<VulkanRenderer>();
    } else if (m_rendererType == "DX12") {
        m_renderer = std::make_unique<DX12Renderer>();
    } else if (m_rendererType == "OpenGL") {
        m_renderer = std::make_unique<GLRenderer>();
    } else {
        std::cerr << "Неизвестный тип рендерера: " << m_rendererType << std::endl;
        return false;
    }

    if (!m_renderer->init(initInfo)) {
        std::cerr << "Ошибка инициализации рендерера" << std::endl;
        return false;
    }

    return true;
}

bool WindowManager::createAppWindows() {
    for (const std::string& type : m_windowTypes) {
        std::unique_ptr<IAppWindow> window;

        if (type == "MainWindow") {
            window = std::make_unique<MainWindow>();
        } else if (type == "EditorWindow") {
            window = std::make_unique<EditorWindow>();
        } else if (type == "ViewportWindow") {
            window = std::make_unique<ViewportWindow>();
        } else {
            std::cerr << "Неизвестный тип окна: " << type << std::endl;
            continue;
        }

        if (window && window->create(m_sdlWindow)) {
            m_appWindows.push_back(std::move(window));
            std::cout << "Создано окно: " << type << std::endl;
        } else {
            std::cerr << "Ошибка создания окна: " << type << std::endl;
        }
    }

    return !m_appWindows.empty();
}

void WindowManager::cleanup() {
    std::cout << "Очистка WindowManager..." << std::endl;
    
    m_appWindows.clear();
    m_renderer.reset();

    if (m_glContext) {
        SDL_GL_DestroyContext(m_glContext);
        m_glContext = nullptr;
    }

    if (m_sdlWindow) {
        SDL_DestroyWindow(m_sdlWindow);
        m_sdlWindow = nullptr;
    }

    SDL_Quit();
}
