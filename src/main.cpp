#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


using namespace std;

class TriangleApp{
    public:

    void run(){
        initWindow();
        initVulkan();
        mainloop();
        cleanup();
    }
    private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    vector<const char*> layerProperties = {"VK_LAYER_KHRONOS_validation"};
   

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan(){
        createInstance();
        setupDebugMessenger();
    }
    void mainloop(){
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
        
    }
    void cleanup(){
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void createInstance(){
        if (enableValidationLayers && !checkValidationLayerSupport()){
            cout<<"Validation layer requested but Not Support\n";
         }

        VkApplicationInfo info{};
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pApplicationName = "Vulkan Engin";
        info.applicationVersion = VK_MAKE_VERSION(1, 0 ,0);
        info.pEngineName = "No Engine";
        info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &info;

        auto allExtensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(allExtensions.size());
        createInfo.ppEnabledExtensionNames = allExtensions.data();
        
        if (enableValidationLayers){
            createInfo.enabledLayerCount = static_cast<uint32_t>(layerProperties.size());
            createInfo.ppEnabledLayerNames = layerProperties.data();
        }else{
            createInfo.enabledLayerCount = 0;
        }

        if(vkCreateInstance(&createInfo,nullptr,&instance) != VK_SUCCESS){
            cout<<"Failed to create instance";
        }

        uint32_t extentionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr,&extentionsCount,nullptr);
        vector<VkExtensionProperties> extensions(extentionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr,&extentionsCount, extensions.data());
        cout<<"Available Extensions:"<<endl;

        for (const auto& prop:extensions){
            cout<<"\t# "<<prop.extensionName<<endl;
        }
    }

    vector<const char*> getRequiredExtensions(){
        uint32_t extenstionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&extenstionCount);
        vector<const char*> extensions(glfwExtensions, glfwExtensions+extenstionCount);

        if (enableValidationLayers){
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport(){
        uint32_t vkValidationLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&vkValidationLayerCount, nullptr);
        vector<VkLayerProperties> availableLayers(vkValidationLayerCount);
        vkEnumerateInstanceLayerProperties(&vkValidationLayerCount, availableLayers.data());

        bool support = true;
        for (auto& layer : layerProperties){
            bool match = false;
            for (auto& available: availableLayers){
                if (strcmp(layer, available.layerName)==0){
                    match = true;
                    break;
                }
            }
            if(!match){
                support = false;
                cout<<"Layer Not Support: "<<layer<<endl;
            }
        }        
        
        return support;
    }

    
    void setupDebugMessenger(){

    }


    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        cerr << "validation layer: " << pCallbackData->pMessage <<endl;
        return VK_FALSE;
    }
};


int main()
{
    cout<<"Starting application...\n";
    
    TriangleApp app;

    try{
        app.run();
    }catch(exception& e){
        cerr<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
