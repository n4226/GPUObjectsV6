#include "Instrumentor.h"
#include "pch.h"



#define RDK_PROFILING 0
#define RDK_PROFILING_LITE 1
#define RDK_VALIDATION 1

#if RDK_PROFILING
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__line__(name);
#define PROFILE_FUNCTION PROFILE_SCOPE(__FUNCSIG__)
#define PROFILE_SCOPE_LEVEL2(name) InstrumentationTimer timer##__line__(name);
#define PROFILE_FUNCTION_LEVEL2 PROFILE_SCOPE_LEVEL2(__FUNCSIG__)
#elif RDK_PROFILING_LITE
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION
#define PROFILE_SCOPE_LEVEL2(name) InstrumentationTimer timer##__line__(name);
#define PROFILE_FUNCTION_LEVEL2 PROFILE_SCOPE_LEVEL2(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION 
#define PROFILE_SCOPE_LEVEL2(name)
#define PROFILE_FUNCTION_LEVEL2
#endif

#if !NDEBUG && RDK_VALIDATION
#define RDX_ENABLE_VK_VALIDATION_LAYERS true;
#else
#define RDX_ENABLE_VK_VALIDATION_LAYERS false;
#endif


// testing environment

//enum RenderingModes
//{
//	modeCPU1 = 0,
//	modeCPU2 = 1,
//	modeGPU = 0,
//};

#define RenderModeCPU1 0
#define RenderModeCPU2 1
#define RenderModeGPU  2

#define RenderMode RenderModeCPU2
//#define RenderMode RenderModeGPU



#define RenderTerainScale 1


#if RenderMode == RenderModeCPU1
#define VK_SUBPASS_INDEX_GBUFFER 0
#define VK_SUBPASS_INDEX_LIGHTING 1
#elif RenderMode == RenderModeCPU2
#define VK_SUBPASS_INDEX_GBUFFER 0
#define VK_SUBPASS_INDEX_LIGHTING 1
#else
#define VK_SUBPASS_INDEX_COMPUTE_PREPASS 0
#define VK_SUBPASS_INDEX_GBUFFER 1
#define VK_SUBPASS_INDEX_LIGHTING 2
#endif

// constants 

/// 100,000
#define maxModelUniformDescriptorArrayCount 100'000

/// 100,000
#define maxMaterialTextureDescriptorArrayCount 100'000
/// normally 10,000
#define FLOATING_ORIGIN_SNAP_DISTANCE 100'000



// used to be in post buiol stage

//XCOPY "$(SolutionDir)MeshGenerator\terrain" "$(TargetDir)\terrain\" /S /Y
//XCOPY "$(SolutionDir)MeshGenerator\terrain" "$(ProjectDir)\terrain\" /S /Y