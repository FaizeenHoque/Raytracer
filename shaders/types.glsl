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