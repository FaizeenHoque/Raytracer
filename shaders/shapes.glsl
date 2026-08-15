layout(std140) uniform SphereBuffer { Sphere spheres[100]; };
uniform int numSpheres;

layout(std140) uniform TriangleBuffer { Triangle triangles[100]; };
uniform int numTriangles;

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
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant >= 0.0) {
        float dst = (-b - sqrt(discriminant)) / (2.0 * a);

        if (dst >= 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphereCentre);
        }
    }

    return hitInfo;
}

HitInfo RayTriangle(Ray ray, Triangle tri, RayTracingMaterial material) {
    vec3 surfaceNormal = normalize(tri.normalA);
    if (dot(ray.dir, surfaceNormal) >= 0.0) {
        HitInfo hitInfo;
        hitInfo.didHit = false;
        return hitInfo;
    }

    vec3 edgeAB = tri.posB - tri.posA;
    vec3 edgeAC = tri.posC - tri.posA;
    vec3 normalVector = cross(edgeAB, edgeAC);
    vec3 ao = ray.origin - tri.posA;
    vec3 dao = cross(ao, ray.dir);

    float determinant = -dot(ray.dir, normalVector);

    if (abs(determinant) < 1E-6) {
        HitInfo hitInfo;
        hitInfo.didHit = false;
        return hitInfo;
    }

    float invDet = 1.0 / determinant;
    float dst = dot(ao, normalVector) * invDet;
    float u = dot(edgeAC, dao) * invDet;
    float v = -dot(edgeAB, dao) * invDet;
    float w = 1.0 - u - v;

    HitInfo hitInfo;
    hitInfo.didHit = dst >= 0.0 && u >= 0.0 && v >= 0.0 && w >= 0.0;

    if (!hitInfo.didHit) return hitInfo;

    hitInfo.dst = dst;
    hitInfo.hitPoint = ray.origin + ray.dir * dst;
    hitInfo.normal = surfaceNormal;

    hitInfo.mat = material;

    return hitInfo;
}

HitInfo CalculateRayCollision(Ray ray) {
    HitInfo closestHit;
    closestHit.didHit = false;
    closestHit.dst = 1.0 / 0.0;
    closestHit.hitPoint = vec3(0.0);
    closestHit.normal = vec3(0.0);
    closestHit.mat.color = vec3(0.0);

    for (int i = 0; i < numSpheres; i++) {
        Sphere sphere = spheres[i];
        HitInfo hitInfo = RaySphere(ray, sphere.position, sphere.radius, sphere.material);

        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) closestHit = hitInfo;
    }

    for (int i = 0; i < numTriangles; i++) {
        Triangle triangle = triangles[i];
        HitInfo hitInfo = RayTriangle(ray, triangle, triangle.material);

        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) closestHit = hitInfo;
    }

    return closestHit;
}
