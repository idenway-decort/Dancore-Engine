#pragma once

#include "../../IAppWindow.h"

class MainWindow : public IAppWindow {
public:
    MainWindow();
    ~MainWindow() override;

    bool create(void* sdlWindow) override;
    void update() override;
    void draw() override;

private:
    void drawMenuBar();
    void onNew();
    void onSaveAs();
    void onExport();
    void onImport();

    bool m_initialized = false;
};
