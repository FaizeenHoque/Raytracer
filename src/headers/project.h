#ifndef RAYTRACER_PROJECT_H
#define RAYTRACER_PROJECT_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "json.hpp"
#include "scene.h"
#include "../objects/headers/Cube.h"
#include "../objects/headers/Quad.h"

using json = nlohmann::json;

struct ProjectSettings {
    float cameraFOV = 45.0f;
    float cameraNear = 0.01f;
    float cameraFar = 1000.0f;
    float cameraYaw = -90.0f;
    float cameraPitch = 0.0f;
    float focusDistance = 100.0f;
    float divergeStrength = 0.2f;
    float defocusStrength = 0.0f;
    int maxBounceCount = 20;
    int numRaysPerPixel = 1;
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -20.0f);
};

inline json Vec3ToJson(const glm::vec3& v) { return json::array({v.x, v.y, v.z}); }
inline json Vec4ToJson(const glm::vec4& v) { return json::array({v.x, v.y, v.z, v.w}); }
inline glm::vec3 JsonToVec3(const json& j) { return glm::vec3(j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>()); }
inline glm::vec4 JsonToVec4(const json& j) { return glm::vec4(j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>(), j.at(3).get<float>()); }

template <typename T>
json MaterialToJson(const T& object) {
    json j;
    j["color"] = Vec4ToJson(object.color);
    j["emissionColor"] = Vec3ToJson(object.emissionColor);
    j["specularColor"] = Vec3ToJson(object.specularColor);
    j["emissionStrength"] = object.emissionStrength;
    j["smoothness"] = object.smoothness;
    j["specularProbability"] = object.specularProbability;
    return j;
}

template <typename T>
void MaterialFromJson(const json& j, T& object) {
    object.color = JsonToVec4(j.at("color"));
    object.emissionColor = JsonToVec3(j.at("emissionColor"));
    object.specularColor = JsonToVec3(j.at("specularColor"));
    object.emissionStrength = j.at("emissionStrength").get<float>();
    object.smoothness = j.at("smoothness").get<float>();
    object.specularProbability = j.at("specularProbability").get<float>();
}

template <typename T>
json MeshObjectToJson(const T& object) {
    json j = MaterialToJson(object);
    j["name"] = object.name;
    j["position"] = Vec3ToJson(object.position);
    j["rotation"] = Vec3ToJson(object.rotation);
    j["scale"] = Vec3ToJson(object.scale);
    return j;
}

template <typename T>
T MeshObjectFromJson(const json& j) {
    T object;
    MaterialFromJson(j, object);
    object.name = j.at("name").get<std::string>();
    object.position = JsonToVec3(j.at("position"));
    object.rotation = JsonToVec3(j.at("rotation"));
    object.scale = JsonToVec3(j.at("scale"));
    return object;
}

inline bool SaveProject(const std::string& path, Scene& scene, const ProjectSettings& settings) {
    json project;
    project["version"] = 1;

    project["settings"] = {
        {"cameraFOV", settings.cameraFOV},
        {"cameraNear", settings.cameraNear},
        {"cameraFar", settings.cameraFar},
        {"cameraYaw", settings.cameraYaw},
        {"cameraPitch", settings.cameraPitch},
        {"focusDistance", settings.focusDistance},
        {"divergeStrength", settings.divergeStrength},
        {"defocusStrength", settings.defocusStrength},
        {"maxBounceCount", settings.maxBounceCount},
        {"numRaysPerPixel", settings.numRaysPerPixel},
		{"cameraPosition", Vec3ToJson(settings.cameraPosition)}
    };

    project["spheres"] = json::array();
    for (const auto& sphere : scene.sphereManager.spheres) {
        json j = MaterialToJson(sphere);
        j["name"] = sphere.name;
        j["position"] = Vec3ToJson(sphere.position);
        j["radius"] = sphere.radius;
        project["spheres"].push_back(j);
    }

    project["cubes"] = json::array();
    for (const auto& cube : scene.cubes) project["cubes"].push_back(MeshObjectToJson(cube));

    project["quads"] = json::array();
    for (const auto& quad : scene.quads) project["quads"].push_back(MeshObjectToJson(quad));

    std::ofstream file(path);
    if (!file) return false;

    file << project.dump(4);
    return file.good();
}

inline bool LoadProject(const std::string& path, Scene& scene, ProjectSettings& settings) {
	std::ifstream file(path);
	if (!file) return false;

	json project = json::parse(file, nullptr, false);
	if (project.is_discarded()) return false;

	try {
		ProjectSettings loaded = settings;
		const json& s = project.at("settings");
		loaded.cameraFOV = s.value("cameraFOV", loaded.cameraFOV);
		loaded.cameraNear = s.value("cameraNear", loaded.cameraNear);
		loaded.cameraFar = s.value("cameraFar", loaded.cameraFar);
		loaded.cameraYaw = s.value("cameraYaw", loaded.cameraYaw);
		loaded.cameraPitch = s.value("cameraPitch", loaded.cameraPitch);
		loaded.focusDistance = s.value("focusDistance", loaded.focusDistance);
		loaded.divergeStrength = s.value("divergeStrength", loaded.divergeStrength);
		loaded.defocusStrength = s.value("defocusStrength", loaded.defocusStrength);
		loaded.maxBounceCount = s.value("maxBounceCount", loaded.maxBounceCount);
		loaded.numRaysPerPixel = s.value("numRaysPerPixel", loaded.numRaysPerPixel);
		if (s.contains("cameraPosition")) loaded.cameraPosition = JsonToVec3(s.at("cameraPosition"));

		std::vector<Sphere> spheres;
		for (const json& j : project.at("spheres")) {
			Sphere sphere("", glm::vec3(0.0f), 1.0f, glm::vec4(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.0f, glm::vec3(0.0f));
			MaterialFromJson(j, sphere);
			sphere.name = j.at("name").get<std::string>();
			sphere.position = JsonToVec3(j.at("position"));
			sphere.radius = j.at("radius").get<float>();
			spheres.push_back(sphere);
		}

		std::vector<Cube> cubes;
		for (const json& j : project.at("cubes")) cubes.push_back(MeshObjectFromJson<Cube>(j));

		std::vector<Quad> quads;
		for (const json& j : project.at("quads")) quads.push_back(MeshObjectFromJson<Quad>(j));

		settings = loaded;
		scene.sphereManager.spheres = spheres;
		scene.sphereManager.Upload();
		scene.cubes = cubes;
		scene.quads = quads;
		scene.RebuildTriangles();
	} catch (const json::exception& e) {
		std::cerr << "Failed to load project: " << e.what() << "\n";
		return false;
	}

	return true;
}

#endif //RAYTRACER_PROJECT_H
