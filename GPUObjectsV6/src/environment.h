#include "Instrumentor.h"
#include "pch.h"

#define RDK_PROFILING 0

#if RDK_PROFILING
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__line__(name);
#define PROFILE_FUNCTION PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION
#endif

#ifdef NDEBUG
#define RDX_ENABLE_VK_VALIDATION_LAYERS false;
#else
#define RDX_ENABLE_VK_VALIDATION_LAYERS true;
#endif


// testing environment

#define RenderMode 2

#define RenderModeCPU1 1
#define RenderModeCPU2 0
#define RenderModeGPU  0

#define RenderTerainScale 1


#if RenderMode == 0
#define VK_SUBPASS_INDEX_GBUFFER 0
#define VK_SUBPASS_INDEX_LIGHTING 1
#elif RenderMode == 1
#define VK_SUBPASS_INDEX_GBUFFER 0
#define VK_SUBPASS_INDEX_LIGHTING 1
#else
#define VK_SUBPASS_INDEX_COMPUTE_PREPASS 0
#define VK_SUBPASS_INDEX_GBUFFER 1
#define VK_SUBPASS_INDEX_LIGHTING 2
#endif

// constants 

/// 100,000
#define maxModelUniformDescriptorArrayCount 100000
/// 10,000 - now 1000
#define FLOATING_ORIGIN_SNAP_DISTANCE 10'000



// used to be in post buiol stage

//XCOPY "$(SolutionDir)MeshGenerator\terrain" "$(TargetDir)\terrain\" /S /Y
//XCOPY "$(SolutionDir)MeshGenerator\terrain" "$(ProjectDir)\terrain\" /S /Y