vec3 GetEnvironmentLight(Ray ray) {
    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.dir.y), 0.35);
    vec3 skyGradient = mix(SkyColourHorizon, SkyColourZenith, skyGradientT);
    float sun = pow(max(0.0, dot(ray.dir, -SunLightDirection)), SunFocus) * SunIntensity;

    float groundToSkyT = smoothstep(-0.01, 0.0, ray.dir.y);
    float sunMask = float(groundToSkyT >= 1.0);

    return mix(GroundColour, skyGradient, groundToSkyT) + sun * sunMask;
}