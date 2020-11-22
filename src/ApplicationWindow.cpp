#include "ApplicationWindow.h"
#include "pch.h"


using namespace std;

WindowManager::WindowManager()
{
    PROFILE_FUNCTION

    createWindow();

    createInstance();

    createSurface();

    createDevice();

    createSwapchain();

    createSwapchainImageViews();

    // make graphics pipeline 
    
    renderPassManager = new RenderPassManager(device, swapchainImageFormat);
    pipelineCreator = new TerrainPipeline(device, swapchainExtent,*renderPassManager);

    pipelineCreator->createGraphicsPipeline();

    createFramebuffers();

    createSemaphores();

    //TODO: Add swap chain recreation for window resizing suport
}

void WindowManager::createDevice()
{
    PROFILE_FUNCTION
    physicalDevice = GPUSelector::primaryGPU(instance, surface);

    queueFamilyIndices = GPUSelector::gpuQueueFamilies(physicalDevice, surface);

    vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamilyIndices.graphicsFamily.value(), 1);

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo queueCreateInfo2(queueCreateInfo);


    //vk::PhysicalDeviceFeatures deviceFeatures();
    VkPhysicalDeviceFeatures deviceFeatures{};

    if (!enableValidationLayers) {
        validationLayers.clear();
    }
    // devie extensions
    const std::vector<const char*> extensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    vk::DeviceCreateFlags flags();

    //vk::DeviceCreateInfo info(flags, 1, &queueCreateInfo, validationLayers.size(), validationLayers.data(), 1 ,extensionNames, &deviceFeatures);

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo2;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();

    createInfo.enabledExtensionCount = extensionNames.size();
    createInfo.ppEnabledExtensionNames = extensionNames.data();

    device = physicalDevice.createDevice(vk::DeviceCreateInfo(createInfo), nullptr);


    device.getQueue(queueFamilyIndices.graphicsFamily.value(),0,&deviceQueues.graphics);
    device.getQueue(queueFamilyIndices.presentFamily.value(), 0,&deviceQueues.presentation);
}

void WindowManager::createSwapchain()
{
    PROFILE_FUNCTION
    SwapChainSupportDetails swapChainSupport = GPUSelector::querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = GPUSelector::chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = GPUSelector::chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = GPUSelector::chooseSwapExtent(swapChainSupport.capabilities,window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }


    assert(queueFamilyIndices.graphicsFamily == queueFamilyIndices.presentFamily);

   /* vk::SwapchainCreateInfoKHR swapInfo = vk::SwapchainCreateInfoKHR(
        vk::SwapchainCreateFlagsKHR{}, surface, imageCount,
        vk::Format(surfaceFormat.format), vk::ColorSpaceKHR(surfaceFormat.colorSpace),
        vk::Extent2D(extent), 1, vk::ImageUsageFlags(vk::ImageUsageFlagBits::eColorAttachment),
        vk::SharingMode::eExclusive, {}, vk::SurfaceTransformFlagBitsKHR(swapChainSupport.capabilities.currentTransform), vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode, vk::Bool32(VK_TRUE),VK_NULL_HANDLE
        );*/

    //device.createSwapchainKHR()

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // this is what needs to change if allowing suport for different presentation and drawing queues
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = VkPresentModeKHR(presentMode);
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    swapChain = device.createSwapchainKHR(vk::SwapchainCreateInfoKHR(createInfo), nullptr);

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
    
}

void WindowManager::createSwapchainImageViews()
{
    PROFILE_FUNCTION
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        swapChainImageViews[i] = device.createImageView({ createInfo });
    }

}

void WindowManager::createFramebuffers()
{
    PROFILE_FUNCTION
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::vector<vk::ImageView> attachments = {
            swapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = renderPassManager->renderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapchainExtent.width;
        framebufferInfo.height = swapchainExtent.height;
        framebufferInfo.layers = 1;

        swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
    }
}

void WindowManager::createSemaphores()
{
    PROFILE_FUNCTION

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo);
        renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo);
        inFlightFences[i] = device.createFence(fenceInfo);
    }
}

void WindowManager::createSurface()
{
    PROFILE_FUNCTION
    VkSurfaceKHR _surface;
    auto result = glfwCreateWindowSurface(instance,window,nullptr,&_surface);

    assert(result == VK_SUCCESS);

    surface = vk::SurfaceKHR(_surface);
}

void WindowManager::createWindow()
{
    PROFILE_FUNCTION
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1920, 1080, "GPUObjectsV6", nullptr, nullptr);
}

void WindowManager::createInstance()
{
    PROFILE_FUNCTION
    auto appInfo = vk::ApplicationInfo(
        "GPUObjectsV6",
        VK_MAKE_VERSION(1,0,0),
        "RenderKit",
        VK_MAKE_VERSION(1,0,0),
        VK_API_VERSION_1_2
    );



    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);


    if (!enableValidationLayers) {
        validationLayers.clear();
    }




    vk::InstanceCreateInfo info = vk::InstanceCreateInfo();

    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = validationLayers.size();
    info.ppEnabledLayerNames = validationLayers.data();
    info.enabledExtensionCount = glfwExtensionCount;
    info.ppEnabledExtensionNames = glfwExtensions;


    instance = vk::createInstance(info);
}

WindowManager::~WindowManager()
{
    PROFILE_FUNCTION
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }


    delete pipelineCreator;
    delete renderPassManager;

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }


    device.destroySwapchainKHR(swapChain);
    device.destroy();

    instance.destroySurfaceKHR(surface);
    instance.destroy();

    destroyWindow();
}

void WindowManager::runWindowLoop()
{
    PROFILE_FUNCTION
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        //drawView();
    }
}


void WindowManager::getDrawable()
{
    PROFILE_FUNCTION

    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    auto index = device.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr);
    cout << "current index = " << index.value << endl;
    currentSurfaceIndex = index.value;

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (imagesInFlight[currentSurfaceIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[currentSurfaceIndex], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    imagesInFlight[currentSurfaceIndex] = inFlightFences[currentFrame];
}

void WindowManager::presentDrawable()
{
    PROFILE_FUNCTION
    // present frame on screen

    vk::PresentInfoKHR presentInfo{};

    std::vector<vk::Semaphore> signalSemaphores = { renderFinishedSemaphores[currentFrame] };

    presentInfo.setWaitSemaphores(signalSemaphores);

    vk::SwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &currentSurfaceIndex;

    presentInfo.pResults = nullptr; // Optional

    deviceQueues.presentation.presentKHR(presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
 }


void WindowManager::destroyWindow()
{
    PROFILE_FUNCTION
    glfwDestroyWindow(window);

    glfwTerminate();
}
