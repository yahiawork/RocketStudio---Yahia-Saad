#pragma once

#include <memory>

#include "Gui.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "../physics/PhysicsEngine.h"
#include "../utils/Timer.h"

struct GLFWwindow;

namespace rs {

class App {
public:
    App();
    ~App();

    int run();

private:
    GLFWwindow* window_ = nullptr;
    PhysicsEngine engine_;
    Renderer2D renderer2D_;
    Renderer3D renderer3D_;
    Gui gui_;
    Timer frameTimer_;

    bool initialize();
    void applyNativeWindowStyle();
    void loadNativeWindowIcon();
    void shutdown();
};

} // namespace rs
