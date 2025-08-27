#pragma once

struct RenderInitInfo {
    void* nativeWindow;
    int width;
    int height;
    const char* appName;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual bool init(const RenderInitInfo& initInfo) = 0;
    virtual void resize(int width, int height) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
};
