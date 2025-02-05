#pragma once

// External Dependancies


#include "windows.h"


#include "vulkan/vulkan.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "mango/mango.hpp"

#include "RenderEngine/vulkanAbstractions/vk_mem_alloc.h"


#include "marl/defer.h"
#include "marl/event.h"
#include "marl/scheduler.h"
#include "marl/waitgroup.h"
#include "marl/task.h"
#include "marl/ticket.h"
#include "marl/mutex.h"
#include "marl/blockingcall.h"

#include "../libGuarded/cs_cow_guarded.h"
#include "../libGuarded/cs_deferred_guarded.h"
#include "../libGuarded/cs_lr_guarded.h"
#include "../libGuarded/cs_ordered_guarded.h"
#include "../libGuarded/cs_shared_guarded.h"
#include "../libGuarded/cs_rcu_guarded.h"

#include <stb-master/stb_image.h>-

// shared code static lib

#include "shared.h"
#include "math/FrustrumMath.h"
#include "math/path/Path.h"
#include "math/path/Bezier.h"
#include "constants.h"

// internal 

#include "RenderEngine/vulkanAbstractions/VkHelpers.h"
#include "Application/ApplicationRendererBridge/ApplicationWindow.h"
#include "math/Math.h"
#include "math/Box.h"
#include "RenderEngine/dataObjects/Mesh.h"
#include "environment.h"
#include "RenderEngine/vulkanAbstractions/Buffer.h"
#include "RenderEngine/vulkanAbstractions/Image.h"
#include "RenderEngine/vulkanAbstractions/Sampler.h"
#include "RenderEngine/dataObjects/uniforms.h"
#include "RenderEngine/systems/renderSystems/RenderSystem.h"
#include "Application/ConfigSystem.h"

// stl
#include <functional>
#include <chrono>
#include <vector>
#include <array>
#include <map>
#include <iostream>
#include <assert.h>
#include <set>
#include <shared_mutex>
#include <optional>
