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