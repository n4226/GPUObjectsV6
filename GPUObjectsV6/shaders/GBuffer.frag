
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable


layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput GBuffer_Albedo_Metallic;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput GBuffer_Normal_Roughness;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput GBuffer_AO;



layout(location = 0) out vec4 outColor;

void main() {
    
    vec4 albedo_metallic =   subpassLoad(GBuffer_Albedo_Metallic);
    vec4 normal_sroughness = subpassLoad(GBuffer_Normal_Roughness);
    vec4 ao =                subpassLoad(GBuffer_AO);

    outColor = vec4(albedo_metallic);
}