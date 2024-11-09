#version 450
#extension GL_ARB_separate_shader_objects : enable

// Inputs
layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec3 InColor;
layout(location = 3) in vec2 InUV;
layout(location = 4) in vec4 InTangent;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 projection;
}
ubo;

// Outputs
layout(location = 0) out vec3 OutColor;
layout(location = 1) out vec2 OutTextCoord;
layout(location = 2) out vec3 OutNormal;
layout(location = 3) out vec3 OutPosition;
layout(location = 4) out vec3 OutCameraPosition;
layout(location = 5) out mat3 OutTBN;

void
main()
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(InPosition, 1.0);
    OutColor = InColor;
    OutTextCoord = InUV;
    OutNormal = InNormal;
    OutPosition = vec3(ubo.model * vec4(InPosition, 1.0));
    OutCameraPosition = vec3(inverse(ubo.view)[3]);

    vec3 bitangent = cross(InNormal, InTangent.xyz);
    vec3 T = normalize(vec3(ubo.model * vec4(InTangent.xyz, 0.0)));
    vec3 B = normalize(vec3(ubo.model * vec4(bitangent.xyz, 0.0)));
    vec3 N = normalize(vec3(ubo.model * vec4(InNormal.xyz, 0.0)));
    OutTBN = transpose(mat3(T, B, N));
}
