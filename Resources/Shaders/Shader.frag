#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform constants
{
    vec3 ambientColor;
    float ambientFactor;
    vec3 lightPosition;
    vec3 lightColor;
}
PushConstants;

void
main()
{
    // Mesh color
    vec3 meshColor = texture(texSampler, fragTexCoord).xyz;

    // Ambient light
    vec3 ambient = PushConstants.ambientColor * PushConstants.ambientFactor;

    // Diffuse light
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(PushConstants.lightPosition - fragPosition);
    float diffuseStrength = max(dot(normal, lightDirection), 0.0) / 2;
    vec3 diffuse = diffuseStrength * PushConstants.lightColor;

    // Result
    vec3 result = (ambient + diffuse) * meshColor;
    float gamma = 2.2;
    outColor = vec4(pow(result, vec3(1.0 / gamma)), 1.0f);
}
