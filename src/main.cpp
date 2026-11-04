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
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "objects/headers/Cube.h"
#include "stb/stb_image_write.h"

// Window
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

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
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    // glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
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

    float FocusDistance = 0.0f;
    float DivergeStrength = 0.2f;
    float DefocusStrength = 0.0f;

    int MaxBounceCount = 20;
    int NumRaysPerPixel = 1;

    Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");
    Camera camera(shaderProgram,
                  CameraFOV,
                  static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
                  CameraNEAR,
                  CameraFAR,
                  FocusDistance,
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

    bool flying = false;
    bool prevRightDown = false;
    bool renderHovered = false;

    int selectedSphere = -1;
    int selectedCube = -1;

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
        ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");

        static bool firstTime = true;

        if (firstTime)
        {
            firstTime = false;

            ImGui::DockBuilderRemoveNode(dockspaceID);

            ImGui::DockBuilderAddNode(
                dockspaceID,
                ImGuiDockNodeFlags_DockSpace
            );

            ImGui::DockBuilderSetNodeSize(
                dockspaceID,
                ImGui::GetMainViewport()->Size
            );

            ImGuiID center = dockspaceID;

            ImGuiID left = ImGui::DockBuilderSplitNode(
                center,
                ImGuiDir_Left,
                0.17f,
                nullptr,
                &center
            );

            ImGuiID right = ImGui::DockBuilderSplitNode(
                center,
                ImGuiDir_Right,
                0.20f,
                nullptr,
                &center
            );

            ImGui::DockBuilderDockWindow("Hierarchy", left);
            ImGui::DockBuilderDockWindow("Properties", right);
            ImGui::DockBuilderDockWindow("Render", center);

            ImGui::DockBuilderFinish(dockspaceID);
        }

        ImGui::DockSpaceOverViewport(dockspaceID, ImGui::GetMainViewport());


        bool rightDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        if (rightDown && !prevRightDown && renderHovered) {
            flying = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if (flying && !rightDown) {
            flying = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        prevRightDown = rightDown;

        bool cameraMoved = scene.camera.Look(window, deltaTime, flying);

        if (cameraMoved)
            renderer.ResetAccumulation();


        bool maxBounceCountChanged = false;
        bool numRaysPerPixelChanged = false;
        bool accumulateChanged = false;
        bool camFovChanged = false;
        bool camNearChanged = false;
        bool camFarChanged = false;
        bool camYawChanged = false;
        bool camPitchChanged = false;
        bool focusChanged = false;
        bool divergeStrengthChanged = false;
        bool defocusStrengthChanged = false;
        bool saveRequested = false;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                ImGui::SeparatorText("Raytracing Settings");

                maxBounceCountChanged = ImGui::InputInt("Max Bounce Count", &MaxBounceCount);
                numRaysPerPixelChanged = ImGui::InputInt("Rays Per Pixel", &NumRaysPerPixel);
                accumulateChanged = ImGui::Checkbox("Accumulate", &enableAccumulation);

                ImGui::SeparatorText("Camera Settings");

                camFovChanged = ImGui::SliderFloat("Camera FOV", &CameraFOV, 10.0f, 120.0f);
                camNearChanged = ImGui::SliderFloat("Camera Near", &CameraNEAR, -100.0f, 100.0f);
                camFarChanged = ImGui::SliderFloat("Camera Far", &CameraFAR, -100.0f, 100.0f);
                camYawChanged = ImGui::SliderFloat("Camera Yaw", &CameraYAW, -180.0f, 180.0f);
                camPitchChanged = ImGui::SliderFloat("Camera Pitch", &CameraPITCH, -180.0f, 180.0f);

                ImGui::Separator();

                focusChanged = ImGui::InputFloat("Focus Distance", &FocusDistance);
                divergeStrengthChanged = ImGui::InputFloat("Diverge Strength", &DivergeStrength);
                defocusStrengthChanged = ImGui::InputFloat("Defocus Strength", &DefocusStrength);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save as PNG"))
                    saveRequested = true;

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Render");
        renderHovered = ImGui::IsWindowHovered();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        float aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
        ImVec2 size = avail;
        if (avail.x / avail.y > aspect) size.x = avail.y * aspect;
        else size.y = avail.x / aspect;

        ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + (avail.x - size.x) * 0.5f, cursor.y + (avail.y - size.y) * 0.5f));
        ImGui::Image((ImTextureID)(intptr_t)renderer.GetOutputTexture(), size, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();

        if (ImGui::Begin("Hierarchy")) {
            auto& spheres = scene.sphereManager.spheres;
            for (int i = 0; i < (int)spheres.size(); i++) {
                ImGui::PushID(i);
                if (ImGui::Selectable(spheres[i].name.c_str(), selectedSphere == i)) { selectedSphere = i; selectedCube = -1; }
                ImGui::PopID();
            }

            for (int i = 0; i < (int)scene.cubes.size(); i++) {
                ImGui::PushID(1000 + i);
                if (ImGui::Selectable(scene.cubes[i].name.c_str(), selectedCube == i)) { selectedCube = i; selectedSphere = -1; }
                ImGui::PopID();
            }

            if (ImGui::BeginPopupContextWindow("More", ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::BeginMenu("Create Object")) {
                    if (ImGui::MenuItem("Sphere")) {
                        Sphere defaultSphere("Default Sphere" ,glm::vec3(0.0f), 5.0f, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.0f, glm::vec3(0.0f));
                        scene.sphereManager.AddSphere(defaultSphere);
                        scene.sphereManager.Upload();
                        renderer.ResetAccumulation();
                    }
                    if (ImGui::MenuItem("Cube")) {
                        Cube cube;
                        cube.name = "Cube";
                        cube.position = glm::vec3(0.0f, 0.0f, 0.0f);
                        cube.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                        cube.scale = glm::vec3(1.0f, 1.0f, 1.0f);
                        cube.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                        cube.emissionColor = glm::vec3(1.0f, 1.0f, 1.0f);
                        cube.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
                        cube.emissionStrength = 0.0f;
                        cube.specularProbability = 0.0f;
                        cube.smoothness = 0.0f;

                        scene.cubes.push_back(Cube());
                        scene.RebuildTriangles();
                        renderer.ResetAccumulation();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Properties")) {
            auto& spheres = scene.sphereManager.spheres;
            if (selectedSphere >= 0 && selectedSphere < (int)spheres.size()) {
                Sphere& sphere = spheres[selectedSphere];
                bool changed = false;

                char nameBuffer[64];
                strncpy(nameBuffer, sphere.name.c_str(), sizeof(nameBuffer) - 1);
                nameBuffer[sizeof(nameBuffer) - 1] = '\0';
                if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) sphere.name = nameBuffer;

                ImGui::Separator();
                ImGui::Text("Transform");
                changed |= ImGui::DragFloat3("Position", &sphere.position.x, 0.1f);
                changed |= ImGui::DragFloat("Radius", &sphere.radius, 0.1f, 0.01f, 1000.0f);

                ImGui::Separator();
                ImGui::Text("Material");
                changed |= ImGui::ColorEdit3("Color", &sphere.color.x);
                changed |= ImGui::ColorEdit3("Emission Color", &sphere.emissionColor.x);
                changed |= ImGui::ColorEdit3("Specular Color", &sphere.specularColor.x);
                ImGui::Separator();
                changed |= ImGui::InputFloat("Emission Strength", &sphere.emissionStrength);
                changed |= ImGui::InputFloat("Smoothness", &sphere.smoothness);
                changed |= ImGui::InputFloat("Specular Probability", &sphere.specularProbability);

                if (changed) {
                    scene.sphereManager.Upload();
                    renderer.ResetAccumulation();
                }
            }

            auto& cubes = scene.cubes;
            if (selectedCube >= 0 && selectedCube < (int)cubes.size()) {
                Cube& cube = cubes[selectedCube];
                bool changed = false;

                char nameBuffer[64];
                strncpy(nameBuffer, cube.name.c_str(), sizeof(nameBuffer) - 1);
                nameBuffer[sizeof(nameBuffer) - 1] = '\0';
                if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) cube.name = nameBuffer;

                ImGui::Separator();
                ImGui::Text("Transform");
                changed |= ImGui::DragFloat3("Position", &cube.position.x, 0.1f);
                changed |= ImGui::DragFloat3("Rotation", &cube.rotation.x, 1.0f);
                changed |= ImGui::DragFloat3("Scale", &cube.scale.x, 0.1f, 0.01f, 1000.0f);

                ImGui::Separator();
                ImGui::Text("Material");
                changed |= ImGui::ColorEdit3("Color", &cube.color.x);
                changed |= ImGui::ColorEdit3("Emission Color", &cube.emissionColor.x);
                changed |= ImGui::ColorEdit3("Specular Color", &cube.specularColor.x);
                changed |= ImGui::InputFloat("Emission Strength", &cube.emissionStrength);
                changed |= ImGui::InputFloat("Smoothness", &cube.smoothness);
                changed |= ImGui::InputFloat("Specular Probability", &cube.specularProbability);

                if (changed) {
                    scene.RebuildTriangles();
                    renderer.ResetAccumulation();
                }
            }
        }
        ImGui::End();

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
