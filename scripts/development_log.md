# Dancore Engine - Журнал разработки и исправления проблем

## Обзор проекта
**Dancore Engine** - закрытый C++23 игровой движок с поддержкой:
- Мультиплатформенности (Windows, Linux, macOS)
- Нескольких рендеринг-бэкендов (Vulkan, DirectX 12, OpenGL)
- Современного редактора на базе Dear ImGui с docking/viewport
- JSON конфигурации через nlohmann/json
- SDL3 для управления окнами и ввода

---

## Хронология проблем и решений

### Проблема 1: Бесконечная сборка SDL3 из исходников
**Описание:** При попытке собрать SDL3 из git submodule возникала ошибка с отсутствующим файлом `SDL_syscamera.h` в dummy camera модуле, что приводило к бесконечной сборке.

**Детали ошибки:**
```
fatal error: '../SDL_syscamera.h' file not found
#include "../SDL_syscamera.h"
         ^~~~~~~~~~~~~~~~~~~~
```

**Попытки решения:**
1. Отключение camera модуля через CMake:
   ```cmake
   set(SDL_CAMERA OFF CACHE BOOL "Enable camera subsystem" FORCE)
   set(SDL_DUMMYCAMERA OFF CACHE BOOL "Enable dummy camera driver" FORCE)
   ```
   - Результат: Не помогло, ошибки линковки продолжались

2. Установка системного SDL3 через Homebrew:
   ```bash
   brew install sdl3
   brew install pkg-config
   ```

**Окончательное решение:**
Переключились на системный SDL3 вместо сборки из исходников:

```cmake
# Было:
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/CMakeLists.txt")
    add_subdirectory(libs/SDL)
else()
    find_package(SDL3 CONFIG REQUIRED)
endif()

# Стало:
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
if(NOT TARGET SDL3::SDL3)
    add_library(SDL3::SDL3 ALIAS PkgConfig::SDL3)
endif()
```

---

### Проблема 2: Ошибки ImGui с DPI scale и Vulkan backend
**Описание:** Приложение крашилось с assertion failed:
```
Assertion failed: (g.CurrentDpiScale > 0.0f && g.CurrentDpiScale < 99.0f), 
function SetCurrentViewport, file imgui.cpp, line 16230
```

**Анализ причины:**
- Использовался неполный Vulkan backend для ImGui
- Vulkan контекст не был правильно инициализирован
- DPI scale не устанавливался корректно без полной Vulkan инициализации

**Попытки решения:**
1. Временное отключение Vulkan backend:
   ```cpp
   // ImGui_ImplVulkan_NewFrame(); // Временно отключено
   ```
   - Результат: Assertion failed продолжался

2. Попытка использования SDL renderer backend:
   ```cmake
   ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
   ```
   - Результат: Backend не существовал или работал нестабильно

**Окончательное решение:**
Переключились на OpenGL backend как наиболее стабильный:

```cmake
# В CMakeLists.txt:
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp  # Заменили на OpenGL
)

find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC SDL3::SDL3 OpenGL::GL)
```

---

### Проблема 3: Неправильный размер окна
**Описание:** Окно открывалось почти на весь экран (1600x900), что было неудобно для разработки.

**Решение:**
Изменили конфигурацию в `configs/startup.json`:
```json
{
  "appName": "Dancore Engine",
  "width": 1280,    // было 1600
  "height": 720,    // было 900
  "renderer": "Vulkan",
  "vsync": true
}
```

---

### Проблема 4: Черный экран и отсутствие ImGui интерфейса
**Описание:** После запуска приложения отображался черный экран без интерфейса ImGui.

**Анализ причины:**
- Неправильная инициализация ImGui backend'ов
- Отсутствие OpenGL контекста
- Неправильная последовательность вызовов рендеринга

**Детальное решение:**

#### 4.1 Изменения в EditorWindow.h:
```cpp
// Было:
struct SDL_Renderer;
SDL_Renderer* m_sdlRenderer = nullptr;

// Стало:
typedef void* SDL_GLContext;
SDL_GLContext m_glContext = nullptr;
```

#### 4.2 Полная переработка инициализации в EditorWindow.cpp:
```cpp
bool EditorWindow::initImGui(void* sdlWindow) {
    SDL_Window* window = static_cast<SDL_Window*>(sdlWindow);
    
    // Создаем OpenGL контекст (НОВОЕ)
    m_glContext = SDL_GL_CreateContext(window);
    if (!m_glContext) {
        std::cerr << "Ошибка создания OpenGL контекста: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Включаем VSync (НОВОЕ)
    SDL_GL_SetSwapInterval(1);
    
    // Создаем контекст ImGui
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imguiContext);
    
    ImGuiIO& io = ImGui::GetIO();
    
    // Включаем только Docking (без ViewportsEnable для стабильности)
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Настраиваем стиль
    ImGui::StyleColorsDark();
    
    // Инициализируем SDL3 backend для OpenGL (ИЗМЕНЕНО)
    if (!ImGui_ImplSDL3_InitForOpenGL(window, m_glContext)) {
        std::cerr << "Ошибка инициализации ImGui SDL3 backend" << std::endl;
        return false;
    }
    
    // Инициализируем OpenGL3 backend (НОВОЕ)
    const char* glsl_version = "#version 130";
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "Ошибка инициализации ImGui OpenGL3 backend" << std::endl;
        return false;
    }
    
    return true;
}
```

#### 4.3 Изменения в рендеринге:
```cpp
void EditorWindow::update() {
    if (!m_initialized) return;
    
    // Начинаем новый кадр ImGui (ИЗМЕНЕНО)
    ImGui_ImplOpenGL3_NewFrame();  // было ImGui_ImplSDLRenderer3_NewFrame()
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    setupDocking();
}

void EditorWindow::draw() {
    if (!m_initialized) return;
    
    // Очищаем экран OpenGL (ИЗМЕНЕНО)
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);  // Серо-голубой фон
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рендер ImGui (ИЗМЕНЕНО)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Показываем результат (ИЗМЕНЕНО)
    SDL_GL_SwapWindow(m_sdlWindow);  // было SDL_RenderPresent()
}
```

#### 4.4 Обновление cleanup функции:
```cpp
void EditorWindow::cleanupImGui() {
    if (m_initialized) {
        ImGui_ImplOpenGL3_Shutdown();  // было ImGui_ImplSDLRenderer3_Shutdown()
        ImGui_ImplSDL3_Shutdown();
        
        if (m_imguiContext) {
            ImGui::DestroyContext(m_imguiContext);
            m_imguiContext = nullptr;
        }
        
        if (m_glContext) {  // было m_sdlRenderer
            SDL_GL_DestroyContext(m_glContext);
            m_glContext = nullptr;
        }
        
        m_initialized = false;
    }
}
```

---

### Проблема 5: Настройка SDL для OpenGL
**Описание:** SDL не был настроен для создания OpenGL контекста.

**Решение в WindowManager.cpp:**
```cpp
bool WindowManager::createWindow() {
    // Настраиваем атрибуты OpenGL (НОВОЕ)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;  // Добавили SDL_WINDOW_OPENGL

    m_sdlWindow = SDL_CreateWindow(
        m_appName.c_str(),
        m_width, m_height,
        windowFlags
    );
    
    // ... остальной код
}
```

---

### Проблема 6: Обработка событий ImGui
**Описание:** События мыши и клавиатуры не передавались в ImGui.

**Решение в WindowManager.cpp:**
```cpp
#include <imgui_impl_sdl3.h>  // Добавили include

void WindowManager::mainLoop() {
    SDL_Event event;
    while (m_running) {
        while (SDL_PollEvent(&event)) {
            // Передаем события в ImGui (НОВОЕ)
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_running = false;
                    break;
                // ... остальная обработка
            }
        }
        
        // ... остальной код цикла
    }
}
```

---

### Проблема 7: Проблемы с терминалом и символами прерывания
**Описание:** В терминале накапливались символы `^C`, что приводило к некорректному выполнению команд.

**Проявление:**
```
^Ccmake -S . -B build -G "Unix Makefiles"
^Cclear
^Cpwd
```

**Решение:**
Создали shell скрипт для автоматизации сборки:

```bash
#!/bin/bash
# build.sh

echo "Сборка Dancore Engine..."

# Удаляем старую build директорию
rm -rf build

# Создаем новую конфигурацию
cmake -S . -B build -G "Unix Makefiles"

# Собираем проект
cmake --build build

echo "Сборка завершена!"
```

---

## Итоговая архитектура

### Структура проекта:
```
DancoreEngine/
├── engine/
│   ├── interfaces/
│   │   ├── WindowManager/          # Менеджер окон с SDL + OpenGL
│   │   └── windows/                # ImGui окна с docking
│   │       ├── MainWindow/         # Меню File
│   │       ├── EditorWindow/       # Док-спейс с OpenGL рендерингом
│   │       └── ViewportWindow/     # Плавающее 3D окно
│   └── render/
│       ├── interfaces/             # Абстрактный IRenderer
│       ├── vulkan/                 # Vulkan заглушка
│       ├── dx12/                   # DirectX 12 заглушка
│       └── opengl/                 # OpenGL заглушка
├── configs/                        # JSON конфигурации
├── libs/                          # Внешние зависимости (ImGui, nlohmann/json)
├── scripts/                       # Скрипты сборки и документация
└── main_files/                    # Точка входа
```

### Технологический стек:
- **Язык:** C++23
- **Окна/Ввод:** SDL3 (системный через Homebrew)
- **GUI:** Dear ImGui (docking branch) с OpenGL3 backend
- **Рендеринг:** OpenGL 3.0 Core Profile
- **Конфигурация:** nlohmann/json (single header)
- **Сборка:** CMake 3.20+

### Ключевые особенности реализации:
1. **Единый SDL окно** с OpenGL контекстом
2. **ImGui docking** для плавающих панелей
3. **Модульная архитектура** с четким разделением интерфейсов
4. **JSON конфигурация** для гибкой настройки
5. **Заглушки рендереров** для будущего расширения

---

## Команды для сборки и запуска

### Подготовка зависимостей (один раз):
```bash
# macOS
brew install sdl3 pkg-config vulkan-tools molten-vk
./scripts/setup_libs.sh
```

### Сборка:
```bash
cd /Users/idenway/Documents/DancoreEngine
rm -rf build
cmake -S . -B build
cmake --build build
```

### Запуск:
```bash
./build/dancore_app
```

---

## Ожидаемый результат

После успешной сборки и запуска должно появиться:
- ✅ Окно размером 1280x720
- ✅ Серо-голубой фон (не черный)
- ✅ Меню File с пунктами New, Save As, Export, Import
- ✅ Док-спейс ImGui на всю область окна
- ✅ Плавающее окно "Viewport" внутри док-спейса
- ✅ Возможность перетаскивания и докирования окон
- ✅ Корректная обработка событий мыши и клавиатуры

---

## Возможные будущие улучшения

1. **Полная реализация Vulkan backend** для ImGui
2. **Добавление Multi-Viewport поддержки** (отстыковка окон)
3. **Реальный 3D рендеринг** в ViewportWindow
4. **Интеграция с системами сцены и ассетов**
5. **Добавление дополнительных панелей редактора** (Inspector, Hierarchy, Console)

---

*Документ создан: 27 августа 2024*
*Последнее обновление: 27 августа 2024*

