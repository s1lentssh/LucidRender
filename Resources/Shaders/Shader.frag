#version 450
#extension GL_ARB_separate_shader_objects : enable
#define PI 3.1415926535897932384626433832795

// Inputs
layout(location = 0) in vec3 InColor;
layout(location = 1) in vec2 InTextureCoordinate;
layout(location = 2) in vec3 InNormal;
layout(location = 3) in vec3 InWorldPosition;
layout(location = 4) in vec3 InCameraPosition;
layout(location = 5) in mat3 InTBN;

layout(binding = 1) uniform sampler2D InAlbedoSampler;
layout(binding = 2) uniform sampler2D InMetallicRoughnessSampler;
layout(binding = 3) uniform sampler2D InNormalSampler;
layout(binding = 4) uniform Material
{
    vec4 color;
    float roughness;
    float metalness;
}
material;

layout(push_constant) uniform constants
{
    vec4 ambientColor;
    vec4 lightPosition;
    vec4 lightColor;
}
PushConstants;

// Outputs
layout(location = 0) out vec4 FragColor;

// Logic
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

void
main()
{
    // Calculate inputs
    vec3 albedo = texture(InAlbedoSampler, InTextureCoordinate).xyz;
    vec3 metallicRoughness = texture(InMetallicRoughnessSampler, InTextureCoordinate).xyz;
    float roughness = metallicRoughness.y;
    float metalness = metallicRoughness.z;

    // Calculate normal
    vec3 normalTexture = texture(InNormalSampler, InTextureCoordinate).rgb;
    // vec3 normal = normalize(normalTexture * 2.0 - 1.0);
    vec3 normal = normalize(InNormal);

    // Hardcode lights
    vec3 lightPositions[] = vec3[](vec3(0.0, 1.0, 3.0));
    vec3 lightColors[] = vec3[](vec3(10.0, 10.0, 10.0));

    vec3 N = normalize(normal);
    vec3 V = normalize(InCameraPosition - InWorldPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalness);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 1; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - InWorldPosition);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - InWorldPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metalness;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.0003) * albedo;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.4));

    FragColor = vec4(color, 1.0);
}

float
DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float
GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float
GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3
FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
