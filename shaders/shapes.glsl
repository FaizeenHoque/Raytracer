layout(std140) uniform SphereBuffer {
    Sphere spheres[100];
};

uniform int numSpheres = 6;

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