#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform vec2 screenSize;
uniform int MaxBounceCount;
uniform int NumRaysPerPixel;
uniform vec3 viewParams;
uniform mat4 camLocalToWorldMatrix;
uniform vec3 worldSpaceCameraPos;
uniform vec3 SkyColourHorizon;
uniform vec3 SkyColourZenith;
uniform vec3 GroundColour;
uniform vec3 SunLightDirection;
uniform float SunFocus;
uniform float SunIntensity;
uniform sampler2D PreviousFrame;
uniform int NumRenderedFrames;

#include "types.glsl"
#include "random.glsl"
#include "shapes.glsl"
#include "lighting.glsl"

vec3 Trace(Ray ray, inout uint rngState) {
    vec3 incomingLight = vec3(0.0);
    vec3 rayColor = vec3(1.0);

    for (int i = 0; i <= MaxBounceCount; i++) {
        HitInfo hitinfo = CalculateRayCollision(ray);

        if (hitinfo.didHit) {
            ray.origin = hitinfo.hitPoint;
            ray.dir = normalize(hitinfo.normal + RandomDirection(rngState));

            RayTracingMaterial material = hitinfo.mat;
            vec3 emittedLight = material.emissionColor * material.emissionStrength;
            incomingLight += emittedLight * rayColor;
            rayColor *= material.color;
        } else {
            incomingLight += GetEnvironmentLight(ray) * rayColor;
            break;
        }
    }

    return incomingLight;
}

void main() {
    uvec2 numPixels = uvec2(screenSize);
    uvec2 pixelCoord = uvec2(uv * screenSize);
    uint pixelIndex = pixelCoord.y * numPixels.x + pixelCoord.x;
    uint rngState = pixelIndex + uint(NumRenderedFrames) * 719393u;

    vec3 viewPointLocal = vec3(uv - 0.5, 1.0) * viewParams;
    vec3 viewPoint = (camLocalToWorldMatrix * vec4(viewPointLocal, 1.0)).xyz;

    Ray ray;
    ray.origin = worldSpaceCameraPos;
    ray.dir = normalize(viewPoint - ray.origin);

    vec3 totalIncomingLight = vec3(0.0);

    for (int rayIndex = 0; rayIndex < NumRaysPerPixel; rayIndex++) {
        totalIncomingLight += Trace(ray, rngState);
    }

    vec3 pixelCol = totalIncomingLight / float(NumRaysPerPixel);
    vec3 previousCol = texture(PreviousFrame, uv).rgb;
    float weight = 1.0 / float(NumRenderedFrames + 1);
    vec3 accumulatedCol = previousCol * (1.0 - weight) + pixelCol * weight;
    FragColor = vec4(accumulatedCol, 1.0);
}