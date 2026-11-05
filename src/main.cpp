#include <algorithm>
#include <ctime>
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
#include "objects/headers/Quad.h"
#include "stb/stb_image_write.h"

#include "headers/project.h"

// Window
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

bool SavePNG(const std::string& filename, int width, int height, GLuint texture) {
	std::vector<unsigned char> pixels(width * height * 4);
	std::vector<unsigned char> flipped(width * height * 4);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	// OpenGL starts at the bottom-left, PNG starts at the top-left
	for (int y = 0; y < height; y++) {
		int src = (height - 1 - y) * width * 4;
		int dst = y * width * 4;
		std::copy_n(pixels.data() + src, width * 4, flipped.data() + dst);
	}

	return stbi_write_png(filename.c_str(), width, height, 4, flipped.data(), width * 4) != 0;
}

void DrawFittedImage(GLuint texture) {
	ImVec2 avail = ImGui::GetContentRegionAvail();
	float aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	ImVec2 size = avail;

	if (avail.x / avail.y > aspect) size.x = avail.y * aspect;
	else size.y = avail.x / aspect;

	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursor.x + (avail.x - size.x) * 0.5f, cursor.y + (avail.y - size.y) * 0.5f));
	ImGui::Image((ImTextureID)(intptr_t)texture, size, ImVec2(0, 1), ImVec2(1, 0));
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
	glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
	// glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!window) {
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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	bool enableAccumulation = true;

	float CameraFOV = 45.0f;
	float CameraNEAR = 0.01f;
	float CameraFAR = 1000.0f;
	float CameraYAW = -90.0f;
	float CameraPITCH = 0.0f;

	float FocusDistance = 100.0f;
	float DivergeStrength = 0.2f;
	float DefocusStrength = 0.0f;

	int MaxBounceCount = 20;
	int NumRaysPerPixel = 1;

	Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");
	Camera camera(
		shaderProgram,
		CameraFOV,
		static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
		CameraNEAR,
		CameraFAR,
		FocusDistance,
		CameraYAW,
		CameraPITCH,
		static_cast<float>(DivergeStrength * std::pow(10, -2)),
		static_cast<float>(DefocusStrength)
	);
	Scene scene(shaderProgram, camera, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	scene.Setup();

	// Initialize renderer
	renderer renderer(shaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);
	renderer.maxBounceCount = MaxBounceCount;
	renderer.numRaysPerPixel = NumRaysPerPixel;

	bool flying = false;
	bool prevRightDown = false;
	bool renderHovered = false;

	int selectedObject = -1;
	float selectedType = 0;

	bool rendering = false;
	int targetSamples = 256;

	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glUseProgram(shaderProgram.ID);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "EnableAccumulation"), enableAccumulation ? 1 : 0);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");

		static bool firstTime = true;

		if (firstTime) {
			firstTime = false;

			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->Size);

			ImGuiID center = dockspaceID;
			ImGuiID left = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.17f, nullptr, &center);
			ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.20f, nullptr, &center);

			ImGui::DockBuilderDockWindow("Hierarchy", left);
			ImGui::DockBuilderDockWindow("Properties", right);
			ImGui::DockBuilderDockWindow("Scene", center);
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
		CameraYAW = scene.camera.yaw;
		CameraPITCH = scene.camera.pitch;

		if (cameraMoved) renderer.ResetAccumulation();

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
		bool saveProjectRequested = false;
		bool loadProjectRequested = false;

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File", !rendering)) {
				if (ImGui::MenuItem("Save Project", "Ctrl+S")) saveProjectRequested = true;
				if (ImGui::MenuItem("Open Project", "Ctrl+O")) loadProjectRequested = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Options", !rendering)) {
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
			ImGui::EndMainMenuBar();
		}

		std::string projectPath = std::string(std::getenv("HOME")) + "/Downloads/project.json";

		if (!rendering && !io.WantTextInput && io.KeyCtrl) {
			if (ImGui::IsKeyPressed(ImGuiKey_S, false)) saveProjectRequested = true;
			if (ImGui::IsKeyPressed(ImGuiKey_O, false)) loadProjectRequested = true;
		}

		if (saveProjectRequested) {
			ProjectSettings settings = {CameraFOV, CameraNEAR, CameraFAR, scene.camera.yaw, scene.camera.pitch, FocusDistance, DivergeStrength, DefocusStrength, MaxBounceCount, NumRaysPerPixel, scene.camera.position};

			if (SaveProject(projectPath, scene, settings)) std::cout << "Saved " << projectPath << "\n";
			else std::cerr << "Failed to save project!\n";
		}

		if (loadProjectRequested) {
			ProjectSettings settings;

			if (LoadProject(projectPath, scene, settings)) {
				CameraFOV = settings.cameraFOV;
				CameraNEAR = settings.cameraNear;
				CameraFAR = settings.cameraFar;
				CameraYAW = settings.cameraYaw;
				CameraPITCH = settings.cameraPitch;
				FocusDistance = settings.focusDistance;
				DivergeStrength = settings.divergeStrength;
				DefocusStrength = settings.defocusStrength;
				MaxBounceCount = settings.maxBounceCount;
				NumRaysPerPixel = settings.numRaysPerPixel;
				scene.camera.position = settings.cameraPosition;

				camFovChanged = camNearChanged = camFarChanged = camYawChanged = camPitchChanged = true;
				focusChanged = divergeStrengthChanged = defocusStrengthChanged = true;
				maxBounceCountChanged = numRaysPerPixelChanged = true;

				selectedObject = -1;
				selectedType = 0;
			} else {
				std::cerr << "Failed to load project!\n";
			}
		}

		if (camFovChanged) {
			scene.camera.fov = CameraFOV;
		}
		if (camNearChanged) {
			scene.camera.nearPlane = CameraNEAR;
		}
		if (camFarChanged) {
			scene.camera.farPlane = CameraFAR;
		}
		if (camYawChanged) {
			scene.camera.yaw = CameraYAW;
		}
		if (camPitchChanged) {
			scene.camera.pitch = CameraPITCH;
		}

		if (focusChanged) {
			scene.camera.focusDistance = FocusDistance;
		}
		if (divergeStrengthChanged) {
			scene.camera.divergeStrength = DivergeStrength;
		}
		if (defocusStrengthChanged) {
			scene.camera.defocusStrength = DefocusStrength;
		}

		if (maxBounceCountChanged) {
			renderer.maxBounceCount = MaxBounceCount;
		}
		if (numRaysPerPixelChanged) {
			renderer.numRaysPerPixel = NumRaysPerPixel;
		}

		if (cameraMoved || camFovChanged || camNearChanged || camFarChanged || camPitchChanged || camYawChanged
			|| focusChanged || defocusStrengthChanged || divergeStrengthChanged
			|| maxBounceCountChanged || numRaysPerPixelChanged || accumulateChanged) {
			renderer.ResetAccumulation();
		}

		scene.SetupParams();
		scene.camera.OnRenderImage(WINDOW_WIDTH, WINDOW_HEIGHT);
		renderer.Render();

		if (saveRequested) {
			GLuint texture = renderer.hasSnapshot ? renderer.GetSnapshotTexture() : renderer.GetOutputTexture();

			if (SavePNG(std::string(std::getenv("HOME")) + "/Downloads/render.png", WINDOW_WIDTH, WINDOW_HEIGHT, texture)) std::cout << "Saved render.png successfully!\n";
			else std::cerr << "Failed to save PNG!\n";
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene");
		renderHovered = !rendering && ImGui::IsWindowHovered();

		if (rendering) {
			const char* message = "Rendering... (Esc to cancel)";
			ImVec2 avail = ImGui::GetContentRegionAvail();
			ImVec2 textSize = ImGui::CalcTextSize(message);
			ImGui::SetCursorPos(ImVec2((avail.x - textSize.x) * 0.5f, (avail.y - textSize.y) * 0.5f));
			ImGui::TextDisabled("%s", message);
		} else {
			DrawFittedImage(renderer.GetOutputTexture());
		}

		ImGui::End();
		ImGui::PopStyleVar();

		static bool livePreview = true;

		ImGui::Begin("Render");

		if (rendering && renderer.numRenderedFrames >= targetSamples) {
			renderer.CaptureSnapshot();
			rendering = false;
		}

		if (rendering) {
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
				renderer.CaptureSnapshot();
				rendering = false;
			}
		} else {
			ImGui::SetNextItemWidth(120);
			ImGui::InputInt("Samples", &targetSamples);
			if (targetSamples < 1) targetSamples = 1;

			if (ImGui::Button("Render")) {
				enableAccumulation = true;
				renderer.ResetAccumulation();
				rendering = true;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Capture") && (rendering || renderer.hasSnapshot)) {
			GLuint texture = rendering ? renderer.GetOutputTexture() : renderer.GetSnapshotTexture();
			std::string path = std::string(std::getenv("HOME")) + "/Downloads/render_" + std::to_string(std::time(nullptr)) + ".png";

			if (SavePNG(path, WINDOW_WIDTH, WINDOW_HEIGHT, texture)) std::cout << "Saved " << path << "\n";
			else std::cerr << "Failed to save PNG!\n";
		}

		if (rendering) {
			float fraction = std::min(1.0f, (float)renderer.numRenderedFrames / (float)targetSamples);
			std::string label = std::to_string(renderer.numRenderedFrames) + " / " + std::to_string(targetSamples);
			ImGui::ProgressBar(fraction, ImVec2(-1, 0), label.c_str());
			DrawFittedImage(renderer.GetOutputTexture());
		} else if (renderer.hasSnapshot) {
			DrawFittedImage(renderer.GetSnapshotTexture());
		} else {
			ImGui::TextDisabled("Click Render to start.");
		}

		ImGui::End();

		if (ImGui::Begin("Hierarchy")) {
			if (rendering) {
				ImGui::TextDisabled("Locked while rendering.");
			} else {
				int deleteIndex = -1;
				int deleteType = 0;

				auto& spheres = scene.sphereManager.spheres;
				for (int i = 0; i < (int)spheres.size(); i++) {
					ImGui::PushID(i);
					bool isSelected = (selectedType == 1 && selectedObject == i);

					if (ImGui::Selectable(spheres[i].name.c_str(), isSelected)) {
						selectedObject = i;
						selectedType = 1;
					}

					if (ImGui::BeginPopupContextItem()) {
						if (ImGui::MenuItem("Delete")) {
							deleteIndex = i;
							deleteType = 1;
						}
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}

				for (int i = 0; i < (int)scene.cubes.size(); i++) {
					ImGui::PushID(1000 + i);
					bool isSelected = (selectedType == 2 && selectedObject == i);

					if (ImGui::Selectable(scene.cubes[i].name.c_str(), isSelected)) {
						selectedObject = i;
						selectedType = 2;
					}

					if (ImGui::BeginPopupContextItem()) {
						if (ImGui::MenuItem("Delete")) {
							deleteIndex = i;
							deleteType = 2;
						}
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}

				for (int i = 0; i < (int)scene.quads.size(); i++) {
					ImGui::PushID(2000 + i);
					bool isSelected = (selectedType == 3 && selectedObject == i);

					if (ImGui::Selectable(scene.quads[i].name.c_str(), isSelected)) {
						selectedObject = i;
						selectedType = 3;
					}

					if (ImGui::BeginPopupContextItem()) {
						if (ImGui::MenuItem("Delete")) {
							deleteIndex = i;
							deleteType = 3;
						}
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}

				if (deleteIndex >= 0) {
					if (deleteType == 1) {
						spheres.erase(spheres.begin() + deleteIndex);
						scene.sphereManager.Upload();
					} else if (deleteType == 2) {
						scene.cubes.erase(scene.cubes.begin() + deleteIndex);
						scene.RebuildTriangles();
					} else if (deleteType == 3) {
						scene.quads.erase(scene.quads.begin() + deleteIndex);
						scene.RebuildTriangles();
					}

					if (selectedType == deleteType) {
						if (selectedObject == deleteIndex) {
							selectedObject = -1;
							selectedType = 0;
						} else if (selectedObject > deleteIndex) {
							selectedObject--;
						}
					}

					renderer.ResetAccumulation();
				}

				if (ImGui::BeginPopupContextWindow("More", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
					if (ImGui::BeginMenu("Create Object")) {
						if (ImGui::MenuItem("Quad")) {
							Quad quad;
							quad.position = glm::vec3(0.0f, 0.0f, 0.0f);
							quad.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
							quad.scale = glm::vec3(20.0f, 3.0f, 20.0f);
							quad.color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
							quad.emissionColor = glm::vec3(1.0f, 1.0f, 1.0f);
							quad.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
							quad.emissionStrength = 0.0f;
							quad.specularProbability = 0.0f;
							quad.smoothness = 0.0f;

							scene.quads.push_back(quad);
							scene.RebuildTriangles();
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

							scene.cubes.push_back(cube);
							scene.RebuildTriangles();
							renderer.ResetAccumulation();
						}

						if (ImGui::MenuItem("Sphere")) {
							Sphere defaultSphere("Default Sphere", glm::vec3(0.0f), 5.0f, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.0f, glm::vec3(0.0f));
							scene.sphereManager.AddSphere(defaultSphere);
							scene.sphereManager.Upload();
							renderer.ResetAccumulation();
						}

						ImGui::EndMenu();
					}

					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();

		if (ImGui::Begin("Properties")) {
			auto& spheres = scene.sphereManager.spheres;
			if ((selectedObject >= 0 && selectedObject < (int)spheres.size()) and selectedType == 1) {
				Sphere& sphere = spheres[selectedObject];
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
			if ((selectedObject >= 0 && selectedObject < (int)cubes.size()) and selectedType == 2) {
				Cube& cube = cubes[selectedObject];
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

			auto& quads = scene.quads;
			if ((selectedObject >= 0 && selectedObject < (int)quads.size()) and selectedType == 3) {
				Quad& quad = quads[selectedObject];
				bool changed = false;

				char nameBuffer[64];
				strncpy(nameBuffer, quad.name.c_str(), sizeof(nameBuffer) - 1);
				nameBuffer[sizeof(nameBuffer) - 1] = '\0';
				if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) quad.name = nameBuffer;

				ImGui::Separator();
				ImGui::Text("Transform");
				changed |= ImGui::DragFloat3("Position", &quad.position.x, 0.1f);
				changed |= ImGui::DragFloat3("Rotation", &quad.rotation.x, 1.0f);
				changed |= ImGui::DragFloat3("Scale", &quad.scale.x, 0.1f, 0.01f, 1000.0f);

				ImGui::Separator();
				ImGui::Text("Material");
				changed |= ImGui::ColorEdit3("Color", &quad.color.x);
				changed |= ImGui::ColorEdit3("Emission Color", &quad.emissionColor.x);
				changed |= ImGui::ColorEdit3("Specular Color", &quad.specularColor.x);
				changed |= ImGui::InputFloat("Emission Strength", &quad.emissionStrength);
				changed |= ImGui::InputFloat("Smoothness", &quad.smoothness);
				changed |= ImGui::InputFloat("Specular Probability", &quad.specularProbability);

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