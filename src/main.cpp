#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

using namespace std;

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

class TriangleApp
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainloop();
        cleanup();
    }

private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;


    vector<const char *> layerProperties = {"VK_LAYER_KHRONOS_validation"};

    void initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
        createLogicalDevice();
    }
    void mainloop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }
    void cleanup()
    {
        vkDestroyDevice(device,nullptr);
        if (enableValidationLayers){
            DestroyDebugUtilsMessengerEXT(instance,debugMessenger, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void createInstance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            cout << "Validation layer requested but Not Support\n";
        }

        VkApplicationInfo info{};
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pApplicationName = "Vulkan Engin";
        info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        info.pEngineName = "No Engine";
        info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &info;

        auto allExtensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(allExtensions.size());
        createInfo.ppEnabledExtensionNames = allExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(layerProperties.size());
            createInfo.ppEnabledLayerNames = layerProperties.data();
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            cout << "Failed to create instance";
        }

        uint32_t extentionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extentionsCount, nullptr);
        vector<VkExtensionProperties> extensions(extentionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extentionsCount, extensions.data());
        cout << "Available Extensions:" << endl;

        for (const auto &prop : extensions)
        {
            cout << "\t# " << prop.extensionName << endl;
        }
    }

    vector<const char *> getRequiredExtensions()
    {
        uint32_t extenstionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&extenstionCount);
        vector<const char *> extensions(glfwExtensions, glfwExtensions + extenstionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport()
    {
        uint32_t vkValidationLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&vkValidationLayerCount, nullptr);
        vector<VkLayerProperties> availableLayers(vkValidationLayerCount);
        vkEnumerateInstanceLayerProperties(&vkValidationLayerCount, availableLayers.data());

        bool support = true;
        for (auto &layer : layerProperties)
        {
            bool match = false;
            for (auto &available : availableLayers)
            {
                if (strcmp(layer, available.layerName) == 0)
                {
                    match = true;
                    break;
                }
            }
            if (!match)
            {
                support = false;
                cout << "Layer Not Support: " << layer << endl;
            }
        }

        return support;
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
    }

    void setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            runtime_error("Failed to setup messenger!");
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        cerr << "validation layer: " << pCallbackData->pMessage << endl;
        return VK_FALSE;
    }

    void pickPhysicalDevice(){
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance,&deviceCount, nullptr);
        vector<VkPhysicalDevice> physicalDevices (deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        for (auto device:physicalDevices){
            if(isDeviceSuitable(device)){
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE){
            runtime_error("Failed to find a GPU!");
        }


    }

    // bool isDeviceSuitable(VkPhysicalDevice device){
    //     return true;
    //     VkPhysicalDeviceProperties deviceProperties;
    //     VkPhysicalDeviceFeatures deviceFeatures;
    //     vkGetPhysicalDeviceProperties(device, &deviceProperties);
    //     vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    //     return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    // }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        return indices.isComplete();
    }

    struct QueueFamilyIndices {
        optional<uint32_t> graphicsFamily;
        bool isComplete() {
            return graphicsFamily.has_value();
        }
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device){
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,nullptr);
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (indices.isComplete()){
                break;
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            i++;
        }
        return indices;
    }

    void createLogicalDevice(){
        float queuePriority = 1.0;
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = & queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;
        if (enableValidationLayers){
            createInfo.enabledLayerCount = static_cast<uint32_t>(layerProperties.size());
            createInfo.ppEnabledLayerNames = layerProperties.data();
        }
        else{
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice,&createInfo,nullptr,&device)!=VK_SUCCESS){
            runtime_error("failed to create logical device!");
        }
        vkGetDeviceQueue(device,indices.graphicsFamily.value(),0,&graphicsQueue);
        
    }

};

int main()
{
    cout << "Starting application...\n";

    TriangleApp app;

    try
    {
        app.run();
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
