#include "../interface/ViewportWindow.h"
#include <imgui.h>
#include <iostream>

ViewportWindow::ViewportWindow() = default;

ViewportWindow::~ViewportWindow() = default;

bool ViewportWindow::create(void* sdlWindow) {
    std::cout << "Создание ViewportWindow..." << std::endl;
    m_initialized = true;
    return true;
}

void ViewportWindow::update() {
    // Логика обновления ViewportWindow
}

void ViewportWindow::draw() {
    if (!m_initialized) return;
    
    // Создаем плавающее окно Viewport внутри DockSpace
    if (ImGui::Begin("Viewport", &m_isOpen, ImGuiWindowFlags_None)) {
        drawViewportContent();
    }
    ImGui::End();
}

void ViewportWindow::drawViewportContent() {
    // Получаем размер области содержимого окна
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    
    // Панель инструментов
    if (ImGui::Button("Reset Camera")) {
        std::cout << "[ViewportWindow] Сброс камеры - заглушка" << std::endl;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload Scene")) {
        std::cout << "[ViewportWindow] Перезагрузка сцены - заглушка" << std::endl;
    }
    ImGui::SameLine();
    ImGui::Text("Size: %.0fx%.0f", viewportSize.x, viewportSize.y - 30);
    
    ImGui::Separator();
    
    // Создаем область для 3D рендеринга
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    
    // Минимальный размер области рендеринга
    if (canvas_sz.x < 100.0f) canvas_sz.x = 100.0f;
    if (canvas_sz.y < 100.0f) canvas_sz.y = 100.0f;
    
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
    
    // Рисуем фон области рендеринга (темно-серый)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(40, 40, 45, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(100, 100, 100, 255));
    
    // Добавляем сетку для наглядности
    ImVec2 center = ImVec2((canvas_p0.x + canvas_p1.x) * 0.5f, (canvas_p0.y + canvas_p1.y) * 0.5f);
    
    // Горизонтальные линии сетки
    for (int i = 1; i < 10; i++) {
        float y = canvas_p0.y + (canvas_sz.y / 10.0f) * i;
        draw_list->AddLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(70, 70, 70, 100));
    }
    
    // Вертикальные линии сетки
    for (int i = 1; i < 10; i++) {
        float x = canvas_p0.x + (canvas_sz.x / 10.0f) * i;
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(70, 70, 70, 100));
    }
    
    // Центральные оси (более яркие)
    draw_list->AddLine(ImVec2(center.x, canvas_p0.y), ImVec2(center.x, canvas_p1.y), IM_COL32(255, 100, 100, 150), 2.0f);
    draw_list->AddLine(ImVec2(canvas_p0.x, center.y), ImVec2(canvas_p1.x, center.y), IM_COL32(100, 255, 100, 150), 2.0f);
    
    // Текст в центре
    ImVec2 text_size = ImGui::CalcTextSize("3D Viewport");
    ImVec2 text_pos = ImVec2(center.x - text_size.x * 0.5f, center.y - text_size.y * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(200, 200, 200, 255), "3D Viewport");
    
    // Обрабатываем клики мыши в области рендеринга
    ImGui::SetCursorScreenPos(canvas_p0);
    ImGui::InvisibleButton("viewport_canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("3D Viewport - здесь будет отображаться сцена");
    }
}
