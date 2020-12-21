#include "pch.h"
#include "Application.h"
#include "WorldScene.h"


Application::Application()
{
    Instrumentor::Get().BeginSession("Launch", "instruments_Launch.profile");

    startup();

    Instrumentor::Get().EndSession();
}

Application::~Application()
{
    Instrumentor::Get().BeginSession("Shutdown", "instruments_Shutdown.profile");
    {
        PROFILE_FUNCTION;


        delete worldScene;



        //Remember to delete all of theme
        for (auto window : windows)
            delete window;
        for (auto renderer : renderers)
            delete renderer;

        scheduler->unbind();

        glfwTerminate();
    }
    Instrumentor::Get().EndSession();
}

void Application::startup()
{
    PROFILE_FUNCTION;


    {// configure main threaqd priority
        auto nativeHandle = GetCurrentThread();
        //SetThreadPriority(nativeHandle, THREAD_PRIORITY_TIME_CRITICAL);
        auto nativeProessHandle = GetCurrentProcess();
        SetPriorityClass(nativeProessHandle, HIGH_PRIORITY_CLASS);
        auto r = GetPriorityClass(nativeProessHandle);
        //printf("d"); 
        //SetPriorityClass()
    }


    {// Configure Marl
        PROFILE_SCOPE("Configutre Marl");
        auto confic = marl::Scheduler::Config();

        confic.setWorkerThreadCount(std::thread::hardware_concurrency() - 1);

        scheduler = new marl::Scheduler(confic);

        scheduler->bind();
    }

    configSystem.readFromDisk();

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    createInstance();

    worldScene = new WorldScene(*this);

    auto cfgWindows = configSystem.global().windows;

        // create windows
    for (size_t i = 0; i < cfgWindows.size(); i++)
    {
        PROFILE_SCOPE("create window");

        auto window = new WindowManager(this,0);

        window->camera.fov = 60;
        window->camera.zNear = 0.1;
        window->camera.zFar = 100'000;//1'000'000;

        windows.push_back(window);


        auto deviceIndex = createDevice(windows.size() - 1);
        
        createRenderer(deviceIndex);
        renderers[deviceIndex]->world = worldScene;

        window->device = devices[deviceIndex];
        window->renderer = renderers[deviceIndex];
        window->finishInit();

        if (deviceIndex == devices.size() - 1) {
            window->indexInRenderer = renderers[deviceIndex]->windows.size();
            renderers[deviceIndex]->windows.push_back(window);

            renderers[deviceIndex]->camFrustroms.emplace_back(windows[i]->camera.view());
        }


        if (window->swapChainImages.size() > maxSwapChainImages) {
            maxSwapChainImages = window->swapChainImages.size();
        }
    }

    for (size_t i = 0; i < renderers.size(); i++)
        renderers[i]->createAllResources();


    worldScene->load();

}

void Application::run()
{
    Instrumentor::Get().BeginSession("Run", "instruments_Run.profile");
    {
        PROFILE_FUNCTION;

        runLoop();
    }
    Instrumentor::Get().EndSession();
}


void Application::runLoop()
{
    worldScene->loadScene();

    //worldScene->updateScene();

    while (shouldLoop()) {
        runLoopIteration();
    }

    windows[0]->device.waitIdle();
}

bool Application::shouldLoop() {
    PROFILE_FUNCTION
    return !glfwWindowShouldClose(windows[0]->window);
}

void Application::runLoopIteration()
{
    PROFILE_FUNCTION;
    {
#if RDK_PROFILING
        std::string s = "loooooooooooooong string";
#endif
        PROFILE_SCOPE("glfwPollEvents");
        glfwPollEvents();
    }
    //drawView();

    // update scene
    worldScene->updateScene();

    renderers[0]->beforeRenderScene();

    // draw view - this should be able to be done all in parallel
    for (size_t i = 0; i < windows.size(); i++)
    {
        if (windows[i]->getDrawable() == true) continue;
        renderers[0]->renderFrame(*windows[i]);
        windows[i]->presentDrawable();
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}



// member funcs


void Application::createRenderer(int deviceIndex)
{
    if (deviceIndex < renderers.size()) return;
    auto index = devices.size() - 1;

    // TODO: fix this because all windows are being added to all devices
    auto renderer = 
        new Renderer(*this,devices[index], physicalDevices[index], allocators[index], windows,deviceQueues[index],queueFamilyIndices[index]);


    renderers.push_back(renderer);

}

int Application::createDevice(int window)
{
    PROFILE_FUNCTION;
    auto physicalDevice = GPUSelector::primaryGPU(instance, windows[window]->surface);

    auto index = std::find(physicalDevices.begin(), physicalDevices.end(), physicalDevice);

    if (index != physicalDevices.end()) {
        return index - physicalDevices.begin();
    }

    auto loc_queueFamilyIndices = GPUSelector::gpuQueueFamilies(physicalDevice, windows[window]->surface);

    const float queuePriority1 = 1.0f;
    vk::DeviceQueueCreateInfo gfxQueueCreateInfo({}, loc_queueFamilyIndices.graphicsFamily.value(), 1);
    gfxQueueCreateInfo.pQueuePriorities = &queuePriority1;
    vk::DeviceQueueCreateInfo transferQueueCreateInfo({}, loc_queueFamilyIndices.resourceTransferFamily.value(), 1);
    transferQueueCreateInfo.pQueuePriorities = &queuePriority1;


    std::array<VkDeviceQueueCreateInfo, 2> queueCreateInfos = { VkDeviceQueueCreateInfo(gfxQueueCreateInfo), VkDeviceQueueCreateInfo(transferQueueCreateInfo) };

    //TODO: add proper way of requesting and falling back on device features
    //vk::PhysicalDeviceFeatures deviceFeatures();
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkPhysicalDeviceDescriptorIndexingFeatures desIndexingFeatures{};
    desIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    desIndexingFeatures.pNext = nullptr;

    desIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
    // not suported by gtx 1080 ti
    //desIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    desIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
    desIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;

    desIndexingFeatures.runtimeDescriptorArray = VK_TRUE;

    // devie extensions
    const std::vector<const char*> extensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    vk::DeviceCreateFlags flags();

    //vk::DeviceCreateInfo info(flags, 1, &queueCreateInfo, validationLayers.size(), validationLayers.data(), 1 ,extensionNames, &deviceFeatures);

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();

    createInfo.enabledExtensionCount = extensionNames.size();
    createInfo.ppEnabledExtensionNames = extensionNames.data();

    // features
    createInfo.pNext = &desIndexingFeatures;

    auto device = physicalDevice.createDevice(vk::DeviceCreateInfo(createInfo), nullptr);

    GPUQueues loc_deviceQueues;

    device.getQueue(loc_queueFamilyIndices.graphicsFamily.value(), 0, &loc_deviceQueues.graphics);
    device.getQueue(loc_queueFamilyIndices.presentFamily.value(), 0, &loc_deviceQueues.presentation);
    device.getQueue(loc_queueFamilyIndices.resourceTransferFamily.value(), 0, &loc_deviceQueues.resourceTransfer);

    devices.push_back(device);
    physicalDevices.push_back(physicalDevice);
    deviceQueues.push_back(loc_deviceQueues);
    queueFamilyIndices.push_back(loc_queueFamilyIndices);

    createAllocator(devices.size() - 1);


    //TODO put this in a better place - fix for multi gpu - put this as a member of renderer so that each gpu has its own resource transferer
    
    return devices.size() - 1;
}


void Application::createAllocator(int deviceIndex)
{
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.physicalDevice = physicalDevices[deviceIndex];
    allocatorInfo.device = devices[deviceIndex];
    allocatorInfo.instance = instance;

    VmaAllocator allocator;
    vmaCreateAllocator(&allocatorInfo,&allocator);

    allocators.push_back(allocator);
}


void Application::createInstance()
{
    PROFILE_FUNCTION
        auto appInfo = vk::ApplicationInfo(
            "GPUObjectsV6",
            VK_MAKE_VERSION(1, 0, 0),
            "RenderKit",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_2
        );



    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;


    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const char* description;
    int code = glfwGetError(&description);

    if (description)
        printf(description);

    vk::InstanceCreateInfo info = vk::InstanceCreateInfo();

    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = validationLayers.size();
    info.ppEnabledLayerNames = validationLayers.data();
    info.enabledExtensionCount = glfwExtensionCount;
    info.ppEnabledExtensionNames = glfwExtensions;


    instance = vk::createInstance(info);
}
