#pragma once

struct SDL_Window;

class IAppWindow {
public:
    virtual ~IAppWindow() = default;
    virtual bool create(void* sdlWindow) = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
};
