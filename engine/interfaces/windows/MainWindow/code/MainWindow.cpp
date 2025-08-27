#include "../interface/MainWindow.h"
#include <imgui.h>
#include <iostream>

MainWindow::MainWindow() = default;

MainWindow::~MainWindow() = default;

bool MainWindow::create(void* sdlWindow) {
    std::cout << "Создание MainWindow..." << std::endl;
    m_initialized = true;
    return true;
}

void MainWindow::update() {
    // Логика обновления MainWindow
}

void MainWindow::draw() {
    if (!m_initialized) return;

    drawMenuBar();
}

void MainWindow::drawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                onNew();
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                onSaveAs();
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Export...")) {
                onExport();
            }
            
            if (ImGui::MenuItem("Import...")) {
                onImport();
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void MainWindow::onNew() {
    std::cout << "[MainWindow] New clicked - пустая заглушка" << std::endl;
}

void MainWindow::onSaveAs() {
    std::cout << "[MainWindow] Save As clicked - пустая заглушка" << std::endl;
}

void MainWindow::onExport() {
    std::cout << "[MainWindow] Export clicked - пустая заглушка" << std::endl;
}

void MainWindow::onImport() {
    std::cout << "[MainWindow] Import clicked - пустая заглушка" << std::endl;
}
