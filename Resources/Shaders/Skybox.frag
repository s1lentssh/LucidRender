#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inUVW;

layout(location = 0) out vec4 outFragColor;

void
main()
{
    float gamma = 2.2;
    outFragColor = vec4(pow(texture(samplerCubeMap, inUVW).rgb, vec3(1.0 / gamma)), 1.0);
}
