#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace UAV
{
class GUIController {
public:
    // Application settings
    struct Controls {
        float rotationY = 0.0f;
        float rotationX = 0.0f;
        float manualRotationY = 0.0f;
        float manualRotationX = 0.0f;
        float rotationSpeed = 0.5f;
        float lightPosX = 0.0f;
        float lightPosY = 0.0f;
        float lightPosZ = 0.0f;
        bool useWireframe = false;
    };
    
    // Constructor
    GUIController(GLFWwindow* window);
    
    // Destructor to clean up ImGui
    ~GUIController();
    
    // Create and render the GUI
    void createGUIWindow();
    
    // Render ImGui frame
    void render();

    bool IsInitialized() const;
    
    // Controls getter
    Controls& getControls() { return controls; }
    
private:
    Controls controls;
};
} // namespace UAV