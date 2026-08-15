const float PI = 3.1415926;

float RandomValue(inout uint state) {
    state = state * 747796405u + 2891336453u;
    uint result = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    result = (result >> 22u) ^ result;
    return float(result) / 4294967295.0;
}

float RandomValueNormalDistribution(inout uint state) {
    float theta = 2 * PI * RandomValue(state);
    float rho = sqrt(-2 * log(max(RandomValue(state), 1e-6)));
    return rho * cos(theta);
}

vec2 RandomPointInCircle(inout uint rngState) {
    float angle = RandomValue(rngState) * 2 * PI;
    vec2 pointOnCircle = vec2(cos(angle), sin(angle));
    return pointOnCircle * sqrt(RandomValue(rngState));
}

vec3 RandomDirection(inout uint state)
{
    float x = RandomValueNormalDistribution(state);
    float y = RandomValueNormalDistribution(state);
    float z = RandomValueNormalDistribution(state);
    return normalize(vec3(x, y, z));
}

vec3 RandomHemisphereDirection(vec3 normal, inout uint rngState) {
    vec3 dir = RandomDirection(rngState);
    return dir * sign(dot(normal, dir));
}
