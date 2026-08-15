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

struct Triangle {
    vec3 posA;
    vec3 posB;
    vec3 posC;
    vec3 normalA;
    vec3 normalB;
    vec3 normalC;
    RayTracingMaterial material;
};