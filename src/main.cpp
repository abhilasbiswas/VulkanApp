#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


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

    uint32_t glfwExtentionCount = 0;
    const char** glfwExtentionNames;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan(){
        createInstance();

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
        VkApplicationInfo info{};
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pApplicationName = "Vulkan Engin";
        info.applicationVersion = VK_MAKE_VERSION(1, 0 ,0);
        info.pEngineName = "No Engine";
        info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &info;

        glfwExtentionNames = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);
        createInfo.enabledExtensionCount = glfwExtentionCount;
        createInfo.ppEnabledExtensionNames = glfwExtentionNames;
        createInfo.enabledLayerCount = 0;
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
