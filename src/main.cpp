#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "headers/scene.h"
#include "headers/camera.h"
#include "headers/shaders.h"
#include "headers/renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Window
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Camera
#define CAMERA_FOV 45.0f
#define CAMERA_NEAR 0.01f
#define CAMERA_FAR 1000.0f
#define CAMERA_YAW -90.0f
#define CAMERA_PITCH 0.0f

#define FOCUS_DISTANCE 20.0f
#define DIVERGE_STRENGTH 0.2
#define DEFOCUS_STRENGTH 200

// Controls
#define SENSITIVITY 0.1f
#define WALK_SPEED 25.0f

// Raytracing
#define MAX_BOUNCE_COUNT 15
#define NUM_RAYS_PER_PIXEL 4

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    bool enableAccumulation = true;
    float CameraFOV = CAMERA_FOV;

    Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");
    Camera camera(shaderProgram,
                  CameraFOV,
                  static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
                  CAMERA_NEAR,
                  CAMERA_FAR,
                  FOCUS_DISTANCE,
                  SENSITIVITY,
                  WALK_SPEED,
                  CAMERA_YAW,
                  CAMERA_PITCH,
                  static_cast<float>(DIVERGE_STRENGTH * std::pow(10, -2)),
                  static_cast<float>(DEFOCUS_STRENGTH));
    Scene scene(shaderProgram, camera, WINDOW_WIDTH, WINDOW_HEIGHT, true);
    scene.Setup();

    // Initialize renderer
    renderer renderer(shaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glUseProgram(shaderProgram.ID);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "EnableAccumulation"),enableAccumulation ? 1 : 0);

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool cameraMoved = scene.camera.Look(window, deltaTime);

        if (cameraMoved)
            renderer.ResetAccumulation();


        ImGui::Begin("Options");
        ImGui::Checkbox("Accumulate", &enableAccumulation);
        bool fovChanged = ImGui::SliderFloat("Camera FOV", &CameraFOV, 10.0f, 120.0f);
        ImGui::End();

        if (fovChanged) { scene.camera.fov = CameraFOV; }
        if (cameraMoved || fovChanged) { renderer.ResetAccumulation(); }

        scene.SetupParams();
        scene.camera.OnRenderImage(WINDOW_WIDTH, WINDOW_HEIGHT);
        renderer.Render(MAX_BOUNCE_COUNT, NUM_RAYS_PER_PIXEL);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    shaderProgram.Delete();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
