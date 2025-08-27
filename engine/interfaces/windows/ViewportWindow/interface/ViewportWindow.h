#pragma once

#include "../../IAppWindow.h"

class ViewportWindow : public IAppWindow {
public:
    ViewportWindow();
    ~ViewportWindow() override;

    bool create(void* sdlWindow) override;
    void update() override;
    void draw() override;

private:
    void drawViewportContent();
    
    bool m_initialized = false;
    bool m_isOpen = true;
};
