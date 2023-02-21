#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inPosition_;
layout(location = 2) in vec3 inPosition__;
layout(location = 3) in vec3 inPosition___;

layout(binding = 0) uniform UBO
{
    mat4 model;
    mat4 view;
    mat4 projection;
}
ubo;

layout(location = 0) out vec3 outUVW;

void
main()
{
    outUVW = inPosition;
    // Convert cubemap coordinates into Vulkan coordinate space
    // outUVW.xy *= -1.0;
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
}