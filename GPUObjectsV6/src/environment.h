#include "Instrumentor.h"
#include "pch.h"

#define RDK_PROFILING 1

#if RDK_PROFILING
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__line__(name);
#define PROFILE_FUNCTION PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION
#endif

#define RenderModeCPU1 1
#define RenderModeCPU2 0
#define RenderModeGPU  0

#ifdef NDEBUG
#define RDX_ENABLE_VK_VALIDATION_LAYERS false;
#else
#define RDX_ENABLE_VK_VALIDATION_LAYERS true;
#endif

// constants 

/// 100,000
#define maxModelUniformDescriptorArrayCount 100000
/// 10,000 - now 1000
#define FLOATING_ORIGIN_SNAP_DISTANCE 10'000