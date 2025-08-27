#pragma once

#include "../../IAppWindow.h"
#include <SDL3/SDL.h>

struct ImGuiContext;

class EditorWindow : public IAppWindow {
public:
    EditorWindow();
    ~EditorWindow() override;

    bool create(void* sdlWindow) override;
    void update() override;
    void draw() override;

private:
    bool initImGui(void* sdlWindow);
    void setupDocking();
    void cleanupImGui();

    ImGuiContext* m_imguiContext = nullptr;
    SDL_Window* m_sdlWindow = nullptr;
    bool m_initialized = false;
};
