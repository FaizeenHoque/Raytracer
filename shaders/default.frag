#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform vec3 viewParams;
uniform mat4 camLocalToWorldMatrix;
uniform vec3 worldSpaceCameraPos;

struct Ray {
    vec3 origin;
    vec3 dir;
};

struct RayTracingMaterial {
    vec4 color;
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

HitInfo RaySphere(
        Ray ray,
        vec3 sphereCentre,
        float sphereRadius,
        RayTracingMaterial material
) {
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
    Sphere spheres[3];
};

uniform int numSpheres = 3;

HitInfo CalculateRayCollision(Ray ray)
{
    HitInfo closestHit;

    closestHit.didHit = false;
    closestHit.dst = 1.0 / 0.0;
    closestHit.hitPoint = vec3(0.0);
    closestHit.normal = vec3(0.0);
    closestHit.mat.color = vec4(0.0);

    for (int i = 0; i < numSpheres; i++)
    {
        Sphere sphere = spheres[i];

        HitInfo hitInfo = RaySphere(
                ray,
                sphere.position,
                sphere.radius,
                sphere.material
        );

        if (hitInfo.didHit && hitInfo.dst < closestHit.dst)
        {
            closestHit = hitInfo;
        }
    }

    return closestHit;
}

void main()
{
    vec3 viewPointLocal =
    vec3(uv - 0.5, 1.0) * viewParams;

    vec4 viewPoint =
    camLocalToWorldMatrix *
    vec4(viewPointLocal, 1.0);

    Ray ray;

    ray.origin = worldSpaceCameraPos;
    ray.dir = normalize(viewPoint.xyz - ray.origin);

    // IMPORTANT: use the sphere buffer here.
    HitInfo hit = CalculateRayCollision(ray);

    if (hit.didHit)
    {
        FragColor = hit.mat.color;
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}