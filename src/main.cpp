#include <algorithm>
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
#include "stb/stb_image_write.h"

// Window
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Controls
#define SENSITIVITY 0.1f
#define WALK_SPEED 25.0f


bool SavePNG(const std::string& filename, int width, int height)
{
    std::vector<unsigned char> pixels(width * height * 4);
    std::vector<unsigned char> flipped(width * height * 4);

    // Read the currently bound framebuffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glReadPixels(
        0, 0,
        width, height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );

    // OpenGL starts at the bottom-left.
    // PNG images start at the top-left.
    for (int y = 0; y < height; y++)
    {
        int src = (height - 1 - y) * width * 4;
        int dst = y * width * 4;

        std::copy_n(
            pixels.data() + src,
            width * 4,
            flipped.data() + dst
        );
    }

    return stbi_write_png(
        filename.c_str(),
        width,
        height,
        4,
        flipped.data(),
        width * 4
    ) != 0;
}


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

    float CameraFOV = 45.0f;
    float CameraNEAR = 0.01f;
    float CameraFAR = 1000.0f;
    float CameraYAW = -90.0f;
    float CameraPITCH = 0.0f;

    float FocusDistance = 20.0f;
    float DivergeStrength = 0.2f;
    float DefocusStrength = 200.0f;

    int MaxBounceCount = 20;
    int NumRaysPerPixel = 1;

    Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");
    Camera camera(shaderProgram,
                  CameraFOV,
                  static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
                  CameraNEAR,
                  CameraFAR,
                  FocusDistance,
                  SENSITIVITY,
                  WALK_SPEED,
                  CameraYAW,
                  CameraPITCH,
                  static_cast<float>(DivergeStrength * std::pow(10, -2)),
                  static_cast<float>(DefocusStrength));
    Scene scene(shaderProgram, camera, WINDOW_WIDTH, WINDOW_HEIGHT, true);
    scene.Setup();

    // Initialize renderer
    renderer renderer(shaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer.maxBounceCount = MaxBounceCount;
    renderer.numRaysPerPixel = NumRaysPerPixel;

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
        ImGui::SeparatorText("Raytracing Settings");
        bool maxBounceCountChanged = ImGui::InputInt("Max Bounce Count", &MaxBounceCount);
        bool numRaysPerPixelChanged = ImGui::InputInt("Rays Per Pixel", &NumRaysPerPixel);
        bool accumulateChanged = ImGui::Checkbox("Accumulate", &enableAccumulation);
        ImGui::SeparatorText("Camera Settings");
        bool camFovChanged = ImGui::SliderFloat("Camera FOV", &CameraFOV, 10.0f, 120.0f);
        bool camNearChanged = ImGui::SliderFloat("Camera Near", &CameraNEAR, -100.0f, 100.0f);
        bool camFarChanged = ImGui::SliderFloat("Camera Far", &CameraFAR, -100.0f, 100.0f);
        bool camYawChanged = ImGui::SliderFloat("Camera Yaw", &CameraYAW, -180.0f, 180.0f);
        bool camPitchChanged = ImGui::SliderFloat("Camera Pitch", &CameraPITCH, -180.0f, 180.0f);
        ImGui::Separator();
        bool focusChanged = ImGui::InputFloat("Focus Distance", &FocusDistance);
        bool divergeStrengthChanged = ImGui::InputFloat("Diverge Strength", &DivergeStrength);
        bool defocusStrengthChanged = ImGui::InputFloat("Defocus Strength", &DefocusStrength);
        ImGui::SeparatorText("Produce");
        bool saveRequested = ImGui::Button("Save as .png");
        ImGui::End();

        if (camFovChanged) { scene.camera.fov = CameraFOV; }
        if (camNearChanged) { scene.camera.nearPlane = CameraNEAR; }
        if (camFarChanged) { scene.camera.farPlane = CameraFAR; }
        if (camYawChanged) { scene.camera.yaw = CameraYAW; }
        if (camPitchChanged) { scene.camera.pitch = CameraPITCH; }

        if (focusChanged) { scene.camera.focusDistance = FocusDistance; }
        if (divergeStrengthChanged) { scene.camera.divergeStrength = DivergeStrength; }
        if (defocusStrengthChanged) { scene.camera.defocusStrength = DefocusStrength; }

        if (maxBounceCountChanged) { renderer.maxBounceCount = MaxBounceCount; }
        if (numRaysPerPixelChanged) { renderer.numRaysPerPixel = NumRaysPerPixel; }

        if (cameraMoved || camFovChanged || camNearChanged || camFarChanged || camPitchChanged || camYawChanged
            || focusChanged || defocusStrengthChanged || divergeStrengthChanged
            || maxBounceCountChanged || numRaysPerPixelChanged || accumulateChanged) { renderer.ResetAccumulation(); }

        scene.SetupParams();
        scene.camera.OnRenderImage(WINDOW_WIDTH, WINDOW_HEIGHT);
        renderer.Render();

        if (saveRequested)
        {
            if (SavePNG(
                (std::string(std::getenv("HOME")) + "/Downloads/render.png"),
                WINDOW_WIDTH,
                WINDOW_HEIGHT
            ))
                std::cout << "Saved render.png successfully!\n";
            else
                std::cerr << "Failed to save PNG!\n";
        }

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
