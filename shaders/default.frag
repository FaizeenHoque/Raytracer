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

struct HitInfo
{
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
};

// Calculate the intersection of a ray with a sphere
HitInfo RaySphere(Ray ray, vec3 sphereCentre, float sphereRadius) {
    HitInfo hitInfo;
    hitInfo.didHit = false;
    hitInfo.dst = 0.0;
    hitInfo.hitPoint = vec3(0.0);
    hitInfo.normal = vec3(0.0);

    vec3 offsetRayOrigin = ray.origin - sphereCentre;
    // From the equation: sqrLength(rayOrigin + rayDir * dst) = radius^2
    // Solving for dst results in a quadratic equation with coefficients:
    float a = dot(ray.dir, ray.dir); // a = 1 (assuming unit vector)
    float b = 2.0 * dot(offsetRayOrigin, ray.dir);
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius;
    // Quadratic discriminant
    float discriminant = b * b - 4.0 * a * c;

    // No solution when d < 0 (ray misses sphere)
    if (discriminant >= 0.0) {
        // Distance to nearest intersection point (from quadratic formula)
        float dst = (-b - sqrt(discriminant)) / (2.0 * a);

        // Ignore intersections that occur behind the ray
        if (dst >= 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphereCentre);
        }
    }
    return hitInfo;
}

void main() {
    vec3 viewPointLocal = vec3(uv - 0.5, 1.0) * viewParams;
    vec4 viewPoint = camLocalToWorldMatrix * vec4(viewPointLocal, 1.0);

    Ray ray;
    ray.origin = worldSpaceCameraPos;
    ray.dir = normalize(viewPoint.xyz - ray.origin);

    HitInfo hit = RaySphere(ray, vec3(0.0), 1.0);
    FragColor = vec4(vec3(hit.didHit ? 1.0 : 0.0), 1.0);
}