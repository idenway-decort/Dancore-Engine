#!/bin/bash

set -e

echo "Настройка зависимостей для Dancore Engine (macOS/Linux)..."

# Создаем директорию libs если её нет
mkdir -p libs

cd libs

# SDL3
echo "Клонирование SDL3..."
if [ ! -d "SDL" ]; then
    git clone --depth 1 https://github.com/libsdl-org/SDL.git SDL
    echo "SDL3 успешно клонирован"
else
    echo "SDL3 уже существует, обновляем..."
    cd SDL
    git pull
    cd ..
fi

# Dear ImGui (docking branch)
echo "Клонирование Dear ImGui (docking branch)..."
if [ ! -d "imgui" ]; then
    git clone --depth 1 --branch docking https://github.com/ocornut/imgui.git imgui
    echo "Dear ImGui успешно клонирован"
else
    echo "Dear ImGui уже существует, обновляем..."
    cd imgui
    git pull
    cd ..
fi

# nlohmann/json
echo "Скачивание nlohmann/json..."
mkdir -p nlohmann_json
cd nlohmann_json
if [ ! -f "json.hpp" ]; then
    curl -L https://github.com/nlohmann/json/releases/latest/download/json.hpp -o json.hpp
    echo "nlohmann/json успешно скачан"
else
    echo "nlohmann/json уже существует"
fi
cd ..

cd ..

echo "Все зависимости настроены!"
echo ""
echo "Для macOS убедитесь, что установлены:"
echo "  brew install vulkan-tools molten-vk"
echo "  или установите Vulkan SDK с https://vulkan.lunarg.com/"
