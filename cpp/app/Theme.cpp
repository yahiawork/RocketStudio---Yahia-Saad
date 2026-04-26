#include "Theme.h"

#include <imgui.h>

namespace rs {

void Theme::apply() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.CellPadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 5.0f);
    style.ScrollbarSize = 14.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    style.WindowRounding = 6.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.93f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.50f, 0.56f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.065f, 0.078f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.075f, 0.087f, 0.105f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.075f, 0.087f, 0.105f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.18f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.105f, 0.125f, 0.150f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.30f, 0.36f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.055f, 0.065f, 0.078f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.075f, 0.087f, 0.105f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.070f, 0.083f, 0.100f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.055f, 0.065f, 0.078f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.27f, 0.33f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.65f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.33f, 0.65f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.22f, 0.78f, 0.64f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.12f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.30f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.44f, 0.54f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.12f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.17f, 0.24f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.21f, 0.34f, 0.42f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.18f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.22f, 0.27f, 0.33f, 0.35f);
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.42f, 0.50f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.17f, 0.24f, 0.30f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.33f, 0.65f, 0.96f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.95f, 0.55f, 0.23f, 1.00f);
}

} // namespace rs
