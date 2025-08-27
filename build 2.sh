#!/bin/bash

echo "=== Сборка Dancore Engine ==="

# Удаляем старую build директорию
echo "Удаление старой build директории..."
rm -rf build

# Создаем новую конфигурацию
echo "Генерация build файлов..."
cmake -S . -B build -G "Unix Makefiles"

if [ $? -ne 0 ]; then
    echo "Ошибка при генерации build файлов!"
    exit 1
fi

# Собираем проект
echo "Компиляция проекта..."
cmake --build build

if [ $? -ne 0 ]; then
    echo "Ошибка при компиляции!"
    exit 1
fi

echo "=== Сборка завершена успешно! ==="
echo "Для запуска используйте: ./build/dancore_app"

