# PowerShell script для настройки зависимостей Dancore Engine (Windows)

Write-Host "Настройка зависимостей для Dancore Engine (Windows)..." -ForegroundColor Green

# Создаем директорию libs если её нет
if (!(Test-Path "libs")) {
    New-Item -ItemType Directory -Path "libs"
}

Set-Location "libs"

# SDL3
Write-Host "Клонирование SDL3..." -ForegroundColor Yellow
if (!(Test-Path "SDL")) {
    git clone --depth 1 https://github.com/libsdl-org/SDL.git SDL
    Write-Host "SDL3 успешно клонирован" -ForegroundColor Green
} else {
    Write-Host "SDL3 уже существует, обновляем..." -ForegroundColor Yellow
    Set-Location "SDL"
    git pull
    Set-Location ".."
}

# Dear ImGui (docking branch)
Write-Host "Клонирование Dear ImGui (docking branch)..." -ForegroundColor Yellow
if (!(Test-Path "imgui")) {
    git clone --depth 1 --branch docking https://github.com/ocornut/imgui.git imgui
    Write-Host "Dear ImGui успешно клонирован" -ForegroundColor Green
} else {
    Write-Host "Dear ImGui уже существует, обновляем..." -ForegroundColor Yellow
    Set-Location "imgui"
    git pull
    Set-Location ".."
}

# nlohmann/json
Write-Host "Скачивание nlohmann/json..." -ForegroundColor Yellow
if (!(Test-Path "nlohmann_json")) {
    New-Item -ItemType Directory -Path "nlohmann_json"
}
Set-Location "nlohmann_json"
if (!(Test-Path "json.hpp")) {
    Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/latest/download/json.hpp" -OutFile "json.hpp"
    Write-Host "nlohmann/json успешно скачан" -ForegroundColor Green
} else {
    Write-Host "nlohmann/json уже существует" -ForegroundColor Yellow
}
Set-Location ".."

Set-Location ".."

Write-Host "Все зависимости настроены!" -ForegroundColor Green
Write-Host ""
Write-Host "Для Windows убедитесь, что установлен Vulkan SDK:" -ForegroundColor Cyan
Write-Host "  https://vulkan.lunarg.com/" -ForegroundColor Cyan
