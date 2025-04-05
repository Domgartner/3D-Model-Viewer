#include "GUIController.h"

namespace UAV
{
GUIController::GUIController(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

GUIController::~GUIController()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUIController::createGUIWindow()
{
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImGui::Begin("Model Viewer Controls");
    
    // Manual rotation controls
    ImGui::SliderFloat("Rotation Y", &controls.manualRotationY, 0.0f, 360.0f);
    ImGui::SliderFloat("Rotation X", &controls.manualRotationX, -90.0f, 90.0f);
    
    // Light controls
    ImGui::Separator();
    ImGui::Text("Light Settings");
    
    ImGui::SliderFloat("Light Position X", &controls.lightPosX, -5.0f, 5.0f);
    ImGui::SliderFloat("Light Position Y", &controls.lightPosY, -5.0f, 5.0f);
    ImGui::SliderFloat("Light Position Z", &controls.lightPosZ, -5.0f, 5.0f);

    ImGui::Separator();
    ImGui::Checkbox("Wireframe Mode", &controls.useWireframe);
    
    ImGui::End();
}

void GUIController::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool GUIController::IsInitialized() const
{
    return ImGui::GetCurrentContext() != nullptr;
}
} // namespace UAV