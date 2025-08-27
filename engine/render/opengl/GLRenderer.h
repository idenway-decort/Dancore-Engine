#pragma once

#include "../interfaces/IRenderer.h"
#include <iostream>

class GLRenderer : public IRenderer {
public:
    bool init(const RenderInitInfo& initInfo) override;
    void resize(int width, int height) override;
    void beginFrame() override;
    void endFrame() override;

private:
    int m_width = 0;
    int m_height = 0;
};
