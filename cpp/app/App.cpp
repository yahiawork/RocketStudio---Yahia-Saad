#include "App.h"

#include <filesystem>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#endif
#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Theme.h"
#include "../io/ConfigIO.h"

namespace rs {
namespace {

#ifdef _WIN32

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif

#ifndef DWMWA_TEXT_COLOR
#define DWMWA_TEXT_COLOR 36
#endif

ULONG_PTR gGdiPlusToken = 0;
HICON gBigIcon = nullptr;
HICON gSmallIcon = nullptr;

bool ensureGdiPlusStarted() {
    if (gGdiPlusToken != 0) {
        return true;
    }

    Gdiplus::GdiplusStartupInput startupInput;
    return Gdiplus::GdiplusStartup(&gGdiPlusToken, &startupInput, nullptr) == Gdiplus::Ok;
}

std::filesystem::path findWindowIconPath() {
    const std::filesystem::path current = std::filesystem::current_path();
    const std::vector<std::filesystem::path> candidates = {
        current / "../assets/icons/favicon.png",
        current / "assets/icons/favicon.png",
        current / "../../assets/icons/favicon.png",
    };

    for (const std::filesystem::path& candidate : candidates) {
        std::error_code ec;
        if (std::filesystem::exists(candidate, ec)) {
            return std::filesystem::weakly_canonical(candidate, ec);
        }
    }

    return {};
}

HICON makeScaledIcon(Gdiplus::Bitmap& source, int width, int height) {
    Gdiplus::Bitmap scaled(width, height, PixelFormat32bppARGB);
    Gdiplus::Graphics graphics(&scaled);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    graphics.DrawImage(&source, 0, 0, width, height);

    HICON icon = nullptr;
    if (scaled.GetHICON(&icon) != Gdiplus::Ok) {
        return nullptr;
    }
    return icon;
}

#endif

} // namespace

App::App() = default;

App::~App() {
    shutdown();
}

int App::run() {
    if (!initialize()) {
        return 1;
    }

    ConfigIO::ensureDefaultConfig("../data/default_config.json");
    SimulationConfig config;
    ConfigIO::loadConfig("../data/default_config.json", config);
    engine_.reset(config);

    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        const double deltaSeconds = frameTimer_.restart();
        engine_.update(deltaSeconds);
        const double fps = deltaSeconds > 1.0e-6 ? 1.0 / deltaSeconds : 0.0;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui_.render(engine_, renderer2D_, renderer3D_, fps);

        ImGui::Render();

        int displayW = 0;
        int displayH = 0;
        glfwGetFramebufferSize(window_, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.035f, 0.043f, 0.055f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
    }

    return 0;
}

bool App::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window_ = glfwCreateWindow(1500,
                               900,
                               "RocketStudio - Developed by Yahia Saad",
                               nullptr,
                               nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    applyNativeWindowStyle();
    loadNativeWindowIcon();

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    Theme::apply();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    return true;
}

void App::applyNativeWindowStyle() {
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window_);
    if (!hwnd) {
        return;
    }

    const BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd,
                          DWMWA_USE_IMMERSIVE_DARK_MODE,
                          &useDarkMode,
                          sizeof(useDarkMode));

    const COLORREF captionColor = RGB(8, 10, 13);
    const COLORREF textColor = RGB(238, 242, 247);
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &captionColor, sizeof(captionColor));
    DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
#endif
}

void App::loadNativeWindowIcon() {
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window_);
    if (!hwnd || !ensureGdiPlusStarted()) {
        return;
    }

    const std::filesystem::path iconPath = findWindowIconPath();
    if (iconPath.empty()) {
        std::cerr << "Window icon not found in assets/icons/favicon.png\n";
        return;
    }

    Gdiplus::Bitmap source(iconPath.wstring().c_str());
    if (source.GetLastStatus() != Gdiplus::Ok) {
        std::cerr << "Failed to load window icon: " << iconPath.string() << '\n';
        return;
    }

    if (gBigIcon) {
        DestroyIcon(gBigIcon);
        gBigIcon = nullptr;
    }
    if (gSmallIcon) {
        DestroyIcon(gSmallIcon);
        gSmallIcon = nullptr;
    }

    gBigIcon = makeScaledIcon(source, 32, 32);
    gSmallIcon = makeScaledIcon(source, 16, 16);

    if (gBigIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(gBigIcon));
    }
    if (gSmallIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(gSmallIcon));
    }
#endif
}

void App::shutdown() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

#ifdef _WIN32
    if (gBigIcon) {
        DestroyIcon(gBigIcon);
        gBigIcon = nullptr;
    }
    if (gSmallIcon) {
        DestroyIcon(gSmallIcon);
        gSmallIcon = nullptr;
    }
    if (gGdiPlusToken != 0) {
        Gdiplus::GdiplusShutdown(gGdiPlusToken);
        gGdiPlusToken = 0;
    }
#endif

    glfwTerminate();
}

} // namespace rs
