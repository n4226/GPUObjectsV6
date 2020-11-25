#pragma once

// External Dependancies

#include "vulkan/vulkan.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vk_mem_alloc.h"


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



// internal 

#include "VkHelpers.h"
#include "ApplicationWindow.h"
#include "Math.h"
#include "Box.h"
#include "Mesh.h"
#include "environment.h"
#include "Buffer.h"
#include "Image.h"
#include "uniforms.h"
#include "RenderSystem.h"

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
