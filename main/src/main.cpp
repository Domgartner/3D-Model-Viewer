#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "camera.h"
#include "model.h"
#include "shader.h"
#include "GUIController.h"

#include <iostream>

namespace UAV
{
// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Global camera instance
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Position closer to see the model
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseCaptured = false;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// GUI Controller for handling ImGui interface
std::unique_ptr<GUIController> guiController = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        mouseCaptured = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    // Capture mouse on left click (when not over ImGui UI)
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse) 
    {
        mouseCaptured = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculate mouse movement delta
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed: y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (mouseCaptured && !ImGui::GetIO().WantCaptureMouse)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
} // namespace UAV

int main()
{
    using namespace UAV;
    
    if (!glfwInit()) 
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Viewer", NULL, NULL);
    if (!window) 
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Set GLFW callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mouseCaptured = true;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    guiController = std::make_unique<GUIController>(window);
    if (!guiController->IsInitialized()) 
    {
        std::cout << "Failed to initialize GUI controller" << std::endl;
        return -1;
    }

    Shader shader("main/shaders/vertex/main.vs", "main/shaders/fragment/main.fs");
    Model model("objects/2025UAV.obj");
    
    // Initial lighting setup
    glm::vec3 lightPos(1.2f, 1.0f, 0.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) 
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        guiController->createGUIWindow();

        // Set wireframe mode if enabled
        if (guiController->getControls().useWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // Get updated values from GUI controller
        GUIController::Controls& controls = guiController->getControls();
        lightPos = glm::vec3(controls.lightPosX, controls.lightPosY, controls.lightPosZ);
        
        shader.use();
        
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("viewPos", camera.Position);
        
        // Create projection and view matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                              static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        
        // Scale model to fit in view
        glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(model.GetScaleFactor()));
        // Center model
        modelMatrix = glm::translate(modelMatrix, -model.GetCenter());
        // Apply rotation from UI controls
        modelMatrix = glm::rotate(modelMatrix, glm::radians(controls.manualRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(controls.manualRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        
        shader.setMat4("model", modelMatrix);
        
        // Draw the model
        model.Draw(shader);
        
        guiController->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}