#version 330 core

in vec2 uv;
out vec4 FragColor;

// Camera
uniform mat4 camLocalToWorldMatrix;
uniform vec3 worldSpaceCameraPos;
uniform vec3 viewParams;
uniform vec2 screenSize;

uniform float DivergeStrength;
uniform float DefocusStrength;

// Ray Tracing
uniform int MaxBounceCount;
uniform int NumRaysPerPixel;

// Accumulation
uniform sampler2D PreviousFrame;
uniform int NumRenderedFrames;

// Lighting
uniform vec3 SunLightDirection;
uniform float SunFocus;
uniform float SunIntensity;

// Sky
uniform vec3 SkyColourHorizon;
uniform vec3 SkyColourZenith;
uniform vec3 GroundColour;

uniform bool showEnvironment;

#include "types.glsl"
#include "random.glsl"
#include "shapes.glsl"

vec3 GetEnvironmentLight(Ray ray) {
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.dir.y), 0.35);
    vec3 skyGradient = mix(SkyColourHorizon, SkyColourZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.dir, -SunLightDirection)), SunFocus) * SunIntensity;

    float groundToSkyT = smoothstep(-0.01, 0.0, ray.dir.y);
    float sunMask = float(groundToSkyT >= 1.0);

    return mix(GroundColour, skyGradient, groundToSkyT) + sun * sunMask;
}

vec3 Trace(Ray ray, inout uint rngState) {
    vec3 incomingLight = vec3(0.0);
    vec3 rayColor = vec3(1.0);
    const float RAY_ORIGIN_EPSILON = 0.001;
    const int RUSSIAN_ROULETTE_START_BOUNCE = 5;

    for (int i = 0; i <= MaxBounceCount; i++) {
        HitInfo hitinfo = CalculateRayCollision(ray);
        RayTracingMaterial material = hitinfo.mat;

        if (hitinfo.didHit) {
            ray.origin = hitinfo.hitPoint + hitinfo.normal * RAY_ORIGIN_EPSILON;
            vec3 diffuseDir = normalize(hitinfo.normal + RandomDirection(rngState));
            vec3 specularDir = reflect(ray.dir, hitinfo.normal);
            bool isSpecularBounce = material.specularProbability >= RandomValue(rngState);
            ray.dir = mix(diffuseDir, specularDir, material.smoothness * float(isSpecularBounce));

            vec3 emittedLight = material.emissionColor * material.emissionStrength;
            incomingLight += emittedLight * rayColor;
            rayColor *= mix(material.color, material.specularColor, isSpecularBounce);

            // The room keeps rays inside the triangle geometry, so nearly every
            // pixel otherwise performs all 50 bounces.  Russian roulette keeps
            // the 50-bounce maximum while terminating dim paths early. Dividing
            // by the survival probability preserves the expected contribution.
            if (i >= RUSSIAN_ROULETTE_START_BOUNCE) {
                float survivalProbability = clamp(max(rayColor.r, max(rayColor.g, rayColor.b)), 0.05, 0.95);
                if (RandomValue(rngState) > survivalProbability) {
                    break;
                }
                rayColor /= survivalProbability;
            }
        } else {
            if (showEnvironment == true)
                incomingLight += GetEnvironmentLight(ray) * rayColor;
            else
                incomingLight *= 0;
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

    vec3 totalIncomingLight = vec3(0.0);

    for (int rayIndex = 0; rayIndex < NumRaysPerPixel; rayIndex++) {
        Ray ray;
        vec2 defocusJitter = RandomPointInCircle(rngState) * DefocusStrength / numPixels.x;
        ray.origin = worldSpaceCameraPos +
            camLocalToWorldMatrix[0].xyz * defocusJitter.x +
            camLocalToWorldMatrix[1].xyz * defocusJitter.y;
        vec2 jitter = RandomPointInCircle(rngState) * DivergeStrength / numPixels.x;
        vec3 jitteredViewPoint = viewPoint +
            camLocalToWorldMatrix[0].xyz * jitter.x +
            camLocalToWorldMatrix[1].xyz * jitter.y;
        ray.dir = normalize(jitteredViewPoint - ray.origin);

        totalIncomingLight += Trace(ray, rngState);
    }

    vec3 pixelCol = totalIncomingLight / float(NumRaysPerPixel);
    vec3 previousCol = texture(PreviousFrame, uv).rgb;
    float weight = 1.0 / float(NumRenderedFrames + 1);
    vec3 accumulatedCol = previousCol * (1.0 - weight) + pixelCol * weight;
    FragColor = vec4(accumulatedCol, 1.0);
}
