
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outAlbedo_Metallic;
layout(location = 1) out vec4 outNormal_Roughness;
layout(location = 2) out vec4 outAO;

void main() {
    outAlbedo_Metallic = vec4(fragColor, 1.0);
}