
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 viewProjection;
} ubo;

layout( push_constant ) uniform DrawPushData {
  uint modelIndex;
} drawData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

layout(location = 0) out vec3 fragColor;

//vec2 positions[3] = vec2[](
//    vec2(0.0, -0.5),
//    vec2(0.5, 0.5),
//    vec2(-0.5, 0.5)
//);
//
//vec3 colors[3] = vec3[](
//    vec3(1.0, 0.3, 0.0),
//    vec3(0.2, 1.0, 0.0),
//    vec3(0.0, 0.1, 1.0)
//);

void main() {
    gl_Position = ubo.viewProjection * vec4(inPosition, 1.0);
    fragColor = inNormal;
}