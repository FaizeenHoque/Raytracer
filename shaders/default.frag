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

struct Ray {
    vec3 origin;
    vec3 dir;
};

struct RayTracingMaterial {
    vec3 color;
    vec3 emissionColor;
    float emissionStrength;
};

struct HitInfo {
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
    RayTracingMaterial mat;
};

struct Sphere {
    vec3 position;
    float radius;
    RayTracingMaterial material;
};

HitInfo RaySphere(Ray ray, vec3 sphereCentre, float sphereRadius, RayTracingMaterial material) {
    HitInfo hitInfo;

    hitInfo.didHit = false;
    hitInfo.dst = 0.0;
    hitInfo.hitPoint = vec3(0.0);
    hitInfo.normal = vec3(0.0);
    hitInfo.mat = material;

    vec3 offsetRayOrigin = ray.origin - sphereCentre;

    float a = dot(ray.dir, ray.dir);
    float b = 2.0 * dot(offsetRayOrigin, ray.dir);
    float c = dot(offsetRayOrigin, offsetRayOrigin)
    - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0 * a * c;

    if (discriminant >= 0.0) {
        float dst = (-b - sqrt(discriminant)) / (2.0 * a);

        if (dst >= 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal =
            normalize(hitInfo.hitPoint - sphereCentre);
        }
    }

    return hitInfo;
}

layout(std140) uniform SphereBuffer {
    Sphere spheres[100];
};

uniform int numSpheres = 5;

HitInfo CalculateRayCollision(Ray ray)
{
    HitInfo closestHit;

    closestHit.didHit = false;
    closestHit.dst = 1.0 / 0.0;
    closestHit.hitPoint = vec3(0.0);
    closestHit.normal = vec3(0.0);
    closestHit.mat.color = vec3(0.0);

    for (int i = 0; i < numSpheres; i++) {
        Sphere sphere = spheres[i];

        HitInfo hitInfo = RaySphere(
                ray,
                sphere.position,
                sphere.radius,
                sphere.material
        );

        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
            closestHit = hitInfo;
        }
    }

    return closestHit;
}

float RandomValue(inout uint state) {
    state = state * 747796405u + 2891336453u;
    uint result = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    result = (result >> 22u) ^ result;

    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution(inout uint state) {
    float theta = 2 * 3.1415926 * RandomValue(state);
    float randomValue = max(RandomValue(state), 0.000001);
    float rho = sqrt(-2 * log(randomValue));
    return rho * cos(theta);
}

vec3 RandomDirection(inout uint state)
{
    for (int attempt = 0; attempt < 100; attempt++)
    {
        float x = RandomValueNormalDistribution(state) * 2.0 - 1.0;
        float y = RandomValueNormalDistribution(state) * 2.0 - 1.0;
        float z = RandomValueNormalDistribution(state) * 2.0 - 1.0;
        vec3 pointInCube = vec3(x, y, z);
        float sqrDstFromCentre = dot(pointInCube, pointInCube);

        if (sqrDstFromCentre <= 1.0 && sqrDstFromCentre > 0.0) {
            return pointInCube / sqrt(sqrDstFromCentre);
        }
    }
    return vec3(0.0, 1.0, 0.0);
}

vec3 RandomHemisphereDirection(vec3 normal, inout uint rngState) {
    vec3 dir = RandomDirection(rngState);
    return dir * sign(dot(normal, dir));
}

vec3 GetEnvironmentLight(Ray ray) {
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.dir.y), 0.35);
    vec3 skyGradient = mix(SkyColourHorizon, SkyColourZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.dir, -SunLightDirection)), SunFocus) * SunIntensity;

    // Combine ground, sky, and sun
    float groundToSkyT = smoothstep(-0.01, 0.0, ray.dir.y);
    float sunMask = float(groundToSkyT >= 1.0);
    return mix(GroundColour, skyGradient, groundToSkyT) + sun * sunMask;
}

vec3 Trace(Ray ray, int MaxBounceCount, inout uint rngState) {
    vec3 incomingLight = vec3(0.0);
    vec3 rayColor = vec3(1.0);

    for (int i = 0; i <= MaxBounceCount; i++) {
        HitInfo hitinfo = CalculateRayCollision(ray);
        if (hitinfo.didHit) {
            ray.origin = hitinfo.hitPoint;
            ray.dir = RandomHemisphereDirection(hitinfo.normal, rngState);

            RayTracingMaterial material = hitinfo.mat;
            vec3 emittedLight = material.emissionColor * material.emissionStrength;
            float lightStrenght = dot(hitinfo.normal, ray.dir);
            incomingLight += emittedLight * rayColor;
            rayColor *= material.color * lightStrenght;
        } else {
            incomingLight += GetEnvironmentLight(ray) * rayColor;
            break;
        }
    }

    return incomingLight;
}

void main()
{
    // Create seed for random number generator
    uvec2 numPixels = uvec2(screenSize);
    uvec2 pixelCoord = uvec2(uv * screenSize);
    uint pixelIndex = pixelCoord.y * numPixels.x + pixelCoord.x;
    uint rngState = pixelIndex + uint(NumRenderedFrames) * 1129u;

    // Create ray
    vec3 viewPointLocal = vec3(uv - 0.5, 1.0) * viewParams;
    vec3 viewPoint = (camLocalToWorldMatrix * vec4(viewPointLocal, 1.0)).xyz;

    Ray ray;
    ray.origin = worldSpaceCameraPos;
    ray.dir = normalize(viewPoint - ray.origin);

    // Calculate pixel colour
    vec3 totalIncomingLight = vec3(0.0f);

    for (int rayIndex = 0; rayIndex < NumRaysPerPixel; rayIndex++) {
        totalIncomingLight += Trace(ray, MaxBounceCount, rngState);
    }

    vec3 pixelCol = totalIncomingLight / float(NumRaysPerPixel);

    vec3 previousCol = texture(PreviousFrame, uv).rgb;
    float weight = 1.0 / float(NumRenderedFrames + 1);
    vec3 accumulatedCol = previousCol * (1.0 - weight) + pixelCol * weight;

    FragColor = vec4(accumulatedCol, 1.0);
}