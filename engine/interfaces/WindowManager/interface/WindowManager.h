#pragma once

#include <memory>
#include <vector>
#include <string>
#include <SDL3/SDL.h>

// Forward declarations
class IRenderer;
class IAppWindow;

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    bool initFromConfigs(const char* startupPath, const char* windowsPath);
    void mainLoop();

private:
    bool loadStartupConfig(const char* path);
    bool loadWindowsConfig(const char* path);
    bool initSDL();
    bool createWindow();
    bool initRenderer();
    bool createAppWindows();
    void cleanup();

    // Конфигурация
    std::string m_appName;
    int m_width = 1600;
    int m_height = 900;
    std::string m_rendererType;
    bool m_vsync = true;

    // SDL и рендеринг
    SDL_Window* m_sdlWindow = nullptr;
    SDL_GLContext m_glContext = nullptr;
    std::unique_ptr<IRenderer> m_renderer;

    // Окна приложения
    std::vector<std::unique_ptr<IAppWindow>> m_appWindows;
    std::vector<std::string> m_windowTypes;

    bool m_running = false;
};
