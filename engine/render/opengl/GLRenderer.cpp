#include "GLRenderer.h"

bool GLRenderer::init(const RenderInitInfo& initInfo) {
    m_width = initInfo.width;
    m_height = initInfo.height;
    
    std::cout << "[OpenGL] Инициализация рендерера для " << initInfo.appName 
              << " (" << m_width << "x" << m_height << ")" << std::endl;
    
    // Здесь будет настоящая инициализация OpenGL
    return true;
}

void GLRenderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    std::cout << "[OpenGL] Изменение размера: " << width << "x" << height << std::endl;
}

void GLRenderer::beginFrame() {
    // Заглушка для начала кадра
}

void GLRenderer::endFrame() {
    // Заглушка для завершения кадра
}
