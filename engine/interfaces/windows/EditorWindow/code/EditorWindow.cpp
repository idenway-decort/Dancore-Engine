#include "../interface/EditorWindow.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <iostream>

EditorWindow::EditorWindow() = default;

EditorWindow::~EditorWindow() {
    cleanupImGui();
}

bool EditorWindow::create(void* sdlWindow) {
    std::cout << "Создание EditorWindow..." << std::endl;
    
    m_sdlWindow = static_cast<SDL_Window*>(sdlWindow);
    
    if (!initImGui(sdlWindow)) {
        std::cerr << "Ошибка инициализации ImGui" << std::endl;
        return false;
    }
    
    m_initialized = true;
    std::cout << "EditorWindow создан успешно" << std::endl;
    return true;
}

void EditorWindow::update() {
    if (!m_initialized) return;
    
    // Начинаем новый кадр ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    setupDocking();
}

void EditorWindow::draw() {
    if (!m_initialized) return;
    
    // Устанавливаем viewport под текущее окно
    int w, h;
    SDL_GetWindowSize(m_sdlWindow, &w, &h);
    glViewport(0, 0, w, h);

    // Очищаем экран серо-синим цветом (как указано в требованиях)
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рендер ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Показываем результат
    SDL_GL_SwapWindow(m_sdlWindow);
}

bool EditorWindow::initImGui(void* sdlWindow) {
    SDL_Window* window = static_cast<SDL_Window*>(sdlWindow);
    m_sdlWindow = window;
    
    std::cout << "Инициализация ImGui контекста..." << std::endl;
    
    // Создаем контекст ImGui
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imguiContext);
    
    ImGuiIO& io = ImGui::GetIO();
    
    // Включаем Docking (без ViewportsEnable для стабильности)
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Настраиваем стиль
    ImGui::StyleColorsDark();
    
    std::cout << "Инициализация ImGui SDL3 backend..." << std::endl;
    // Инициализируем SDL3 backend (используем уже созданный OpenGL контекст)
    if (!ImGui_ImplSDL3_InitForOpenGL(window, SDL_GL_GetCurrentContext())) {
        std::cerr << "Ошибка инициализации ImGui SDL3 backend" << std::endl;
        return false;
    }
    
    std::cout << "Инициализация ImGui OpenGL3 backend..." << std::endl;
    // Инициализируем OpenGL3 backend
    const char* glsl_version = "#version 130";
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "Ошибка инициализации ImGui OpenGL3 backend" << std::endl;
        return false;
    }
    
    std::cout << "ImGui успешно инициализирован с OpenGL и Docking поддержкой" << std::endl;
    return true;
}

void EditorWindow::setupDocking() {
    // Создаем полноэкранное окно с DockSpace и меню
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("Dancore Engine Editor", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    // Создаем меню File
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                std::cout << "[EditorWindow] New clicked - пустая заглушка" << std::endl;
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                std::cout << "[EditorWindow] Save As clicked - пустая заглушка" << std::endl;
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Export...")) {
                std::cout << "[EditorWindow] Export clicked - пустая заглушка" << std::endl;
            }
            
            if (ImGui::MenuItem("Import...")) {
                std::cout << "[EditorWindow] Import clicked - пустая заглушка" << std::endl;
            }
            
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    // Создаем DockSpace на всё окно
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("DancoreEngineDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }
    
    ImGui::End();
}

void EditorWindow::cleanupImGui() {
    if (m_initialized) {
        std::cout << "Очистка ImGui..." << std::endl;
        
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        
        if (m_imguiContext) {
            ImGui::DestroyContext(m_imguiContext);
            m_imguiContext = nullptr;
        }
        
        m_initialized = false;
    }
}
