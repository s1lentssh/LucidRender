#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTextCoordinate;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTextCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragPosition;

void main() {
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTextCoord = inTextCoordinate;
    fragNormal = inNormal;
    fragPosition = vec3(ubo.model * vec4(inPosition, 1.0));
}
