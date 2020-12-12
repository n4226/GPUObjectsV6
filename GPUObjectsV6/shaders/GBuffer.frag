
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable



// Lighting math lots of code from ----- 

// this was implemented by following sevral totorials:
// Alan Zucconi: Volumetric Atmospheric Scattering - https://www.alanzucconi.com/?p=7374
// nvidea : ///

//

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308

const float kInfinity = FLT_MAX;

#define earthRadius 6378137
#define atmosphereRadius 6378137 + 60000

// atm constants
// thease are hight scales i believe
#define Hr float(7994)
#define Hm float(1200)

#define betaR float3(3.8e-6f, 13.5e-6f, 33.1e-6f)
#define betaM float3(21e-6f)


bool solveQuadratic(float a, float b, float c, out float x1, out float x2)
{
    if (b == 0) {
        // Handle special case where the the two vector ray.dir and V are perpendicular
        // with V = ray.orig - sphere.centre
        if (a == 0) return false;
        x1 = 0; x2 = sqrt(-c / a);
        return true;
    }
    float discr = b * b - 4 * a * c;
    
    if (discr < 0) return false;
    
    float q = (b < 0.f) ? -0.5f * (b - sqrt(discr)) : -0.5f * (b + sqrt(discr));
    x1 = q / a;
    x2 = c / q;
    
    return true;
}



bool raySphereIntersect(vec3 orig, vec3 dir, float radius, out float t0,out float t1)
{
    // They ray dir is normalized so A = 1
    float A = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    float B = 2 * (dir.x * orig.x + dir.y * orig.y + dir.z * orig.z);
    float C = orig.x * orig.x + orig.y * orig.y + orig.z * orig.z - radius * radius;
    
    if (!solveQuadratic(A, B, C, t0, t1)) {
        return false;
    }
    
    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }
    
    return true;
}










vec3 calculatePostAtmosphereicScatering(
                                          ivec2 textureSize,
                                          vec3 camPos, // without floating origin  // world
                                          mat4x4 ViewMatrix,
                                          vec3 sunDirection
) {

    return vec3(0,0,0.8);

}



// end Lighting math





layout(binding = 3) uniform UniformBufferObject {
// global uniforms
    mat4 viewProjection;

    // post uniforms
    mat4 invertedViewMat;
    mat4 viewMat;

    vec4 earthCenter;
    vec4 sunDir;
    vec4 camFloatedGloabelPos;
} ubo;



layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput GBuffer_Albedo_Metallic;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput GBuffer_Normal_Roughness;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput GBuffer_AO;



layout(location = 0) out vec4 outColor;

void main() {
    
    vec4 albedo_metallic =   subpassLoad(GBuffer_Albedo_Metallic);
    vec4 normal_sroughness = subpassLoad(GBuffer_Normal_Roughness);
    vec4 ao =                subpassLoad(GBuffer_AO);

    if (albedo_metallic.w == 0) {
        albedo_metallic.xyz = calculatePostAtmosphereicScatering(ivec2(0,0),vec3(0),mat4(1),vec3(0,1,0));
        albedo_metallic.w = 1;
    }

    outColor = vec4(albedo_metallic);
}