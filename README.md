# Dancore Engine

Закрытый C++23 игровой движок с поддержкой нескольких рендеринг-бэкендов и современным редактором на базе ImGui.

## Возможности

- **Мультиплатформенность**: Windows, Linux, macOS (с MoltenVK), планируется iPadOS/Android
- **Несколько рендеринг-бэкендов**: Vulkan (по умолчанию), DirectX 12 (Windows), OpenGL (fallback)
- **Современный редактор**: Dear ImGui с поддержкой docking и multi-viewport
- **Конфигурация через JSON**: Легкая настройка через конфигурационные файлы
- **Модульная архитектура**: Четкое разделение интерфейсов и реализаций

## Структура проекта

```
DancoreEngine/
├── engine/
│   ├── interfaces/
│   │   ├── WindowManager/          # Менеджер окон
│   │   └── windows/                # Интерфейсы окон приложения
│   │       ├── MainWindow/         # Главное окно с меню
│   │       ├── EditorWindow/       # Окно редактора с docking
│   │       └── ViewportWindow/     # Окно 3D viewport
│   └── render/
│       ├── interfaces/             # Интерфейсы рендеринга
│       ├── vulkan/                 # Vulkan рендерер
│       ├── dx12/                   # DirectX 12 рендерер
│       └── opengl/                 # OpenGL рендерер
├── configs/                        # JSON конфигурации
├── libs/                          # Внешние зависимости
├── scripts/                       # Скрипты сборки
└── main_files/                    # Точка входа
```

## Зависимости

- **SDL3**: Управление окнами и ввод
- **Dear ImGui**: Пользовательский интерфейс (docking branch)
- **nlohmann/json**: Парсинг JSON конфигураций
- **Vulkan SDK**: Для Vulkan рендеринга (обязательно)
- **MoltenVK**: Для macOS (через Vulkan SDK или Homebrew)

## Сборка

### macOS

1. Установите зависимости:
```bash
# Через Homebrew
brew install vulkan-tools molten-vk

# Или установите Vulkan SDK с https://vulkan.lunarg.com/
```

2. Подготовьте зависимости проекта:
```bash
./scripts/setup_libs.sh
```

3. Соберите проект:
```bash
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

4. Запустите:
```bash
./build/dancore_app
```

### Windows

1. Установите Vulkan SDK с https://vulkan.lunarg.com/

2. Подготовьте зависимости проекта:
```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\setup_libs.ps1
```

3. Соберите проект:
```cmd
cmake -S . -B build
cmake --build build --config Debug
```

4. Запустите:
```cmd
.\build\Debug\dancore_app.exe
```

### Linux

1. Установите зависимости:
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install vulkan-tools libvulkan-dev

# Fedora
sudo dnf install vulkan-tools vulkan-devel

# Arch
sudo pacman -S vulkan-tools vulkan-headers
```

2. Подготовьте зависимости проекта:
```bash
./scripts/setup_libs.sh
```

3. Соберите проект:
```bash
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

4. Запустите:
```bash
./build/dancore_app
```

## Конфигурация

### configs/startup.json
Основные параметры приложения:
```json
{
  "appName": "Dancore Engine",
  "width": 1600,
  "height": 900,
  "renderer": "Vulkan",
  "vsync": true
}
```

### configs/windows.json
Конфигурация окон интерфейса:
```json
{
  "windows": [
    { "type": "MainWindow" },
    { "type": "EditorWindow" },
    { "type": "ViewportWindow" }
  ]
}
```

## Опции CMake

- `DCE_RENDER_VULKAN` (ON): Включить Vulkan рендерер
- `DCE_RENDER_DX12` (AUTO): Включить DirectX 12 рендерер (только Windows)
- `DCE_RENDER_GL` (OFF): Включить OpenGL рендерер

Пример:
```bash
cmake -S . -B build -DDCE_RENDER_GL=ON
```

## Возможности редактора

- **Docking**: Перетаскивание и закрепление окон
- **Multi-Viewport**: Извлечение окон в отдельные окна операционной системы
- **Viewport**: 3D окно просмотра сцены (пока заглушка)
- **Меню File**: Базовые операции (New, Save As, Export, Import)

## Разработка

Движок использует C++23 и современные практики разработки:

- Четкое разделение интерфейсов и реализаций
- RAII и умные указатели
- Модульная архитектура
- Конфигурация через JSON

## Лицензия

Данный проект является закрытым. Все права защищены.
# Dancore-Engine
# Dancore-Engine
