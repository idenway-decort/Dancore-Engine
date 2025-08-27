#include "interfaces/WindowManager/interface/WindowManager.h"
#include <iostream>

int main() {
    std::cout << "=== Запуск Dancore Engine ===" << std::endl;

    WindowManager windowManager;
    
    if (!windowManager.initFromConfigs("configs/startup.json", "configs/windows.json")) {
        std::cerr << "Ошибка инициализации WindowManager" << std::endl;
        return 1;
    }
    
    windowManager.mainLoop();
    
    std::cout << "=== Завершение Dancore Engine ===" << std::endl;
    return 0;
}
