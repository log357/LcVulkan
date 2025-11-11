#include "LcVulkan.hpp"
#include "LcMath.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "./tiny_obj_loader/tiny_obj_loader.h"

LcWindow::LcWindow()
:LcWindow({1200,780,"LcWindow"})
{
}

LcWindow::LcWindow(LcWindowInfo window_info) 
:
    m_Info(window_info),
    m_Window(nullptr),
    m_CurrentFrame(0),
    m_Camera(LcCamera::LOOK_POS,LcCamera::LOOK_FRONT,LcCamera::LOOK_UP),
    m_Fov(45.f),
    m_MsaaSamples(VK_SAMPLE_COUNT_1_BIT),
    m_CursorDisabled(false),
    m_ShowImgui(true),
    m_ShowCube(true),
    m_UseParticleNumber(10000),
    m_Fps(0.f)
{
    LoadModel();
    InitWindow();
    InitVulkan();
    InitCamera();
    InitImgui();

}

void LcWindow::InitWindow(){
    // glfw 尝试初始化
    if(glfwInit() == GLFW_FALSE){
        throw std::runtime_error("Failed to init glfw!");       // glfw初始化失败
    }
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);                // glfw默认支持OpenGL，现在设置不要支持OpenGL，在当前Vulkan环境中，必须将 GLFW_CLIENT_API 设置为 GLFW_NO_API
    glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);                   // 窗口大小调整设定
    // glfw 生成窗体
    m_Window = glfwCreateWindow(m_Info.width,                   // 窗体宽
                                m_Info.height,                  // 窗体高
                                m_Info.title.c_str(),           // 窗体标题
                                nullptr,                        // 指定要全屏显示的监视器。如果为 NULL，则创建窗口模式
                                nullptr                         // 共享 OpenGL 上下文的窗口。如果为 NULL，则不共享上下文
                                );
    if(m_Window == nullptr){
        throw std::runtime_error("Failed to create window!");   // 创建窗体失败 
    }
    glfwSetWindowUserPointer(m_Window,this);
    glfwSetFramebufferSizeCallback(m_Window,FramebufferResizeCallback);
    glfwSetKeyCallback(m_Window, KeyCallback);

}

void LcWindow::LoadModel() {

    // std::vector<LcVertex> torus_vertices ;
    // std::vector<uint32_t> torus_indices;
    // // 参数：外径 1.0, 内径 0.4, 大圆细分 40, 小圆细分 25
    // GenerateTorusMesh_Rainbow(torus_vertices, torus_indices, 1.0f, 0.4f, 40, 25);

    // m_Vertices = torus_vertices;
    // m_Indices = torus_indices;

    std::vector<LcVertex> Vertices = {
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, {0,0}},
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, {1,0}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, {1,1}},
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, {0,1}},
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, {1,0}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, {0,0}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, {0,1}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, {1,1}},
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, {0,0}},
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, {1,0}},
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, {1,1}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, {0,1}},
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, {0,0}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, {1,0}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, {1,1}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, {0,1}},
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, {0,0}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, {1,0}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, {1,1}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, {0,1}},
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, {0,0}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, {1,0}},
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, {1,1}},
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, {0,1}}
    };

    for(auto& val:Vertices){
        val.inPosition*=1;
    }
    std::vector<uint32_t> Indices = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    m_Vertices = Vertices;
    m_Indices = Indices;

    // 网格坐标
    std::vector<LcInstanceData> instances;
    for (int i = 0; i < 11; i++) {
        float y = -10.0f + i * 2.0f;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, y, 0.0f));
        model = glm::scale(model, glm::vec3(20.0f, 0.02f, 0.02f));
        instances.push_back({model});
    }
    for (int i = 0; i < 11; i++) {
        float x = -10.0f + i * 2.0f;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.02f, 20.0f, 0.02f));
        instances.push_back({model});
    }
    m_InstanceDatas = instances;

    // 粒子
    m_Particles.resize(MAX_PARTICLES);
    for (auto &p : m_Particles) {
        // 随机位置在 [-10, 10] 范围内
        p.pos = glm::vec3(
            (rand() / (float)RAND_MAX) * 20.0f,
            (rand() / (float)RAND_MAX) * 20.0f,
            0.f
        );
        // 大小
        p.size = 10;
        // 随机颜色
        p.color = glm::vec4(rand() / (float)RAND_MAX,rand() / (float)RAND_MAX,rand() / (float)RAND_MAX,1.0f);
    }
}
void LcWindow::InitVulkan(){
    // 创建 Vulkan 核心实例
    CreateInstance();
    // 创建 Vulkan 检验层
    SetupMessenger();
    // 创建窗口表面
    CreateSurface();
    // 挑选物理设备
    PickupPhysicalDevice();
    // 创建逻辑设备
    CreateLogicDevice();
    // 创建交换链
    CreateSwapChain();
    // 创建交换链渲染视图
    CreateImageView();
    // 创建渲染通道
    CreateRenderPass();
    // 创建粒子描述符集布局
    CreateParticleDescriptorSetLayout();
    // 创建方块描述符集布局
    CreateCubeDescriptorSetLayout();
    // 创建粒子渲染管线
    CreateParticlePipeline();
    // 创建方块管线
    CreateCubePipeline();
    // 创建命令池
    CreateCommandPool();
    // 创建颜色采样缓存
    CreateColorResources();
    // 创建深度缓存
    CreateDepthResources();
    // 创建帧缓冲区
    CreateFrameBuffers();
    // 创建纹理图像
    CreateTextureImage();
    // 创建纹理图像视图
    CreateTextureImageView();
    // 创建纹理采样器
    CreateTextureSampler();
    // 创建顶点缓冲区
    CreateVertexBuffer();
    // 创建索引缓冲区
    CreateIndexBuffer();
    // 创建实例缓冲区
    CreateLcInstanceBuffers();
    // 创建粒子缓冲区
    CreateParticleBuffers();
    // 创建统一缓冲区对象
    CreateUboBuffers();
    // 创建粒子描述符池
    CreateParticleDescriptorPool();
    // 创建方块描述符池
    CreateCubeDescriptorPool();
    // 创建粒子描述符集
    CreateParticleDescriptorSets();
    // 创建方块描述符集
    CreateCubeDescriptorSets();
    // 创建命令缓冲区
    CreateCommandBuffers();
    // 创建信号量信息
    CreateSyncObjects();
}

void LcWindow::CleanupVulkan(){

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(m_LogicDevice, m_ImguiPool, VK_NULL_HANDLE);                        // 销毁Imgui描述符池
    for(uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT; index++){
        vkDestroySemaphore(m_LogicDevice, m_ImageAvailableSemaphores[index], nullptr);          // 销毁图像可用信号量
        vkDestroyFence(m_LogicDevice, m_InFlightFences[index], nullptr);                        // 销毁飞行中栅栏
    }
    for(uint32_t index = 0; index < m_RenderFinishedSemaphores.size(); index++){
        vkDestroySemaphore(m_LogicDevice, m_RenderFinishedSemaphores[index], nullptr);          // 销毁渲染完成信号量
    }
    CleanupSwapChain();                                                                         // 清除交换链
    for (uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT; index++) {
        vkDestroyBuffer(m_LogicDevice, m_InstanceBuffers[index], nullptr);                      // 销毁正方体实例缓冲区
        vkFreeMemory(m_LogicDevice, m_InstanceMemories[index], nullptr);                        // 销毁正方体实例内存信息
    }

    vkDestroyBuffer(m_LogicDevice, m_ParticleBuffer, nullptr);                                  // 销毁粒子实例缓冲区
    vkFreeMemory(m_LogicDevice, m_ParticleMemory, nullptr);                                     // 销毁粒子实例内存信息
    vkDestroyBuffer(m_LogicDevice,m_ParticleStagingBuffer,nullptr);                             // 销毁粒子实例暂存缓冲区
    vkFreeMemory(m_LogicDevice, m_ParticleStagingMemory, nullptr);                              // 销毁粒子实例暂存内存信息


    vkDestroySampler(m_LogicDevice, m_TextureSampler, nullptr);                                 // 销毁采样器
    vkDestroyImageView(m_LogicDevice, m_TextureImageView, nullptr);                             // 销毁纹理视图
    vkDestroyImage(m_LogicDevice, m_TextureImage, nullptr);                                     // 销毁纹理图像
    vkFreeMemory(m_LogicDevice, m_TextureImageMemory, nullptr);                                 // 销毁纹理图像关联的设备内存
    vkDestroyBuffer(m_LogicDevice,m_VertexBuffer,nullptr);                                      // 销毁顶点缓冲区
    vkFreeMemory(m_LogicDevice,m_VertexMemory,nullptr);                                         // 销毁顶点内存信息
    vkDestroyBuffer(m_LogicDevice,m_IndexBuffer,nullptr);                                       // 销毁索引缓冲区
    vkFreeMemory(m_LogicDevice,m_IndexMemory,nullptr);                                          // 销毁索引内存信息
    vkDestroyDescriptorPool(m_LogicDevice, m_ParticleDescriptorPool, nullptr);                  // 销毁粒子描述符池
    vkDestroyDescriptorPool(m_LogicDevice, m_CubeDescriptorPool, nullptr);                      // 销毁方块描述符池

    for (uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT; index++) {
        vkDestroyBuffer(m_LogicDevice, m_UboBuffers[index], nullptr);                           // 销毁UBO缓存
        vkFreeMemory(m_LogicDevice, m_UboMemories[index], nullptr);                             // 销毁UBO关联的设备内存
    }
    vkDestroyDescriptorSetLayout(m_LogicDevice, m_ParticleUboSetLayout, nullptr);               // 销毁粒子描述符集布局
    vkDestroyDescriptorSetLayout(m_LogicDevice, m_CubeUboSetLayout, nullptr);                   // 销毁方块描述符集布局

    vkDestroyCommandPool(m_LogicDevice, m_CommandPool, nullptr);                                // 销毁命令池

    vkDestroyPipeline(m_LogicDevice,m_ParticlePipeline,nullptr);                                // 销毁粒子渲染管线
    vkDestroyPipeline(m_LogicDevice,m_CubePipeline,nullptr);                                    // 销毁方块渲染管线
    
    vkDestroyPipelineLayout(m_LogicDevice,m_ParticlePipelineLayout,nullptr);                    // 销毁粒子渲染布局
    vkDestroyPipelineLayout(m_LogicDevice,m_CubePipelineLayout,nullptr);                        // 销毁方块渲染布局

    vkDestroyRenderPass(m_LogicDevice,m_RenderPass,nullptr);                                    // 销毁渲染通道
    vkDestroyDevice(m_LogicDevice,nullptr);                                                     // 销毁逻辑设备
    if(EnabledValidationLayer){
        DestroyDebugUtilsMessenger(m_Instance,m_DebugMessenger,nullptr/*默认Allocation*/);      // 销毁检验层信使
    }
    vkDestroySurfaceKHR(m_Instance,m_Surface,nullptr);                                          // 销毁窗口表面
    vkDestroyInstance(m_Instance,nullptr/*默认Allocation*/);                                    // 销毁核心实例

}
void LcWindow::InitCamera() {
    m_Camera.SetProcessingInput(m_Window);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(m_Window, MouseCallback);
    glfwSetScrollCallback(m_Window, ScrollCallback);
}
void LcWindow::InitImgui(){
    // 准备imgui所需要的descriptor pool
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // ImGui 需要
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(m_LogicDevice, &pool_info, nullptr, &m_ImguiPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Dear ImGui descriptor pool!");
    }
    // 创建imgui的context
    ImGui::CreateContext();

    // 中文字体支持
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("./Fonts/Mengshen-Handwritten.ttf", 27);

    // 设置一些主题样式
    ImGuiStyle * style = &ImGui::GetStyle();

    style->WindowPadding            = ImVec2(15, 15);
    style->WindowRounding           = 5.0f;
    style->FramePadding             = ImVec2(5, 5);
    style->FrameRounding            = 4.0f;
    style->ItemSpacing              = ImVec2(12, 8);
    style->ItemInnerSpacing         = ImVec2(8, 6);
    style->IndentSpacing            = 25.0f;
    style->ScrollbarSize            = 15.0f;
    style->ScrollbarRounding        = 9.0f;
    style->GrabMinSize              = 5.0f;
    style->GrabRounding             = 3.0f;

    style->Colors[ImGuiCol_Text]                  = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.39f, 0.38f, 0.77f);
    style->Colors[ImGuiCol_WindowBg]              = ImVec4(0.92f, 0.91f, 0.88f, 0.70f);
    style->Colors[ImGuiCol_ChildBg]               = ImVec4(1.00f, 0.98f, 0.95f, 0.58f);
    style->Colors[ImGuiCol_PopupBg]               = ImVec4(0.92f, 0.91f, 0.88f, 0.92f);
    style->Colors[ImGuiCol_Border]                = ImVec4(0.84f, 0.83f, 0.80f, 0.65f);
    style->Colors[ImGuiCol_BorderShadow]          = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.99f, 1.00f, 0.40f, 0.78f);
    style->Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TitleBg]               = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg]             = ImVec4(1.00f, 0.98f, 0.95f, 0.47f);
    style->Colors[ImGuiCol_ScrollbarBg]           = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.00f, 0.00f, 0.00f, 0.21f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.90f, 0.91f, 0.00f, 0.78f);
    style->Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_CheckMark]             = ImVec4(0.25f, 1.00f, 0.00f, 0.80f);
    style->Colors[ImGuiCol_SliderGrab]            = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    style->Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_Button]                = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    style->Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.99f, 1.00f, 0.22f, 0.86f);
    style->Colors[ImGuiCol_ButtonActive]          = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_Header]                = ImVec4(0.25f, 1.00f, 0.00f, 0.76f);
    style->Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 1.00f, 0.00f, 0.86f);
    style->Colors[ImGuiCol_HeaderActive]          = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
    style->Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style->Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotLines]             = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    // 设置一个主题颜色
    ImGui::StyleColorsClassic();

    // 绑定具体的surface
    ImGui_ImplGlfw_InitForVulkan(m_Window,true);
    // 绑定相应的Vulkan上下文
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_Instance;
    init_info.PhysicalDevice = m_PhysicalDevice;
    init_info.Device = m_LogicDevice;
    init_info.QueueFamily = m_Family.graphicsQueueFamily.value();
    init_info.Queue = m_GraphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = m_ImguiPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = m_SwapChainImages.size();
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.PipelineInfoMain.RenderPass = m_RenderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = m_MsaaSamples;
    ImGui_ImplVulkan_Init(&init_info);
}


void LcWindow::CreateParticleBuffers() {
    VkDeviceSize bufferSize = sizeof(LcParticle) * MAX_PARTICLES;

    // --- 创建 GPU buffer (DEVICE_LOCAL) ---
    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_ParticleBuffer,
                 m_ParticleMemory);

    // --- 创建 staging buffer (HOST_VISIBLE) 复用 ---
    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 m_ParticleStagingBuffer,
                 m_ParticleStagingMemory);

    vkMapMemory(m_LogicDevice, m_ParticleStagingMemory, 0, bufferSize, 0, &m_ParticleDataMapped);
}


void LcWindow::CreateInstance(){
    // 获取验证层可否使用
    if(EnabledValidationLayer && !CheckValidationLayerSupport()){
        throw std::runtime_error("Validation layer is not available!");                     // 验证层不可用！
    }
    VkApplicationInfo appInfo{};                                                            // 指定应用程序信息的结构体
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;                                     // 必填，扩展链类型标识（与pNext成员解析相关联）
    appInfo.pNext = nullptr;                                                                // 指向特定的扩展结构
    appInfo.pApplicationName = "LcVulkan";                                                  // 应用名称，开发人员提供的应用程序的名称
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);                                    // 应用版本，开发人员提供的应用程序的版本号
    appInfo.apiVersion = VK_API_VERSION_1_4;                                                // 必填，根据Vulkan的最高版本 vulkansdk-windows-X64-1.4.313.2 这里写1.4
    appInfo.pEngineName="No Engine";                                                        // 引擎名称，创建应用程序的引擎的开发人员提供的名称
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);                                         // 引擎版本，创建应用程序的引擎的开发人员提供的版本号
    uint32_t glfwExtentionCount;                                                            // 实例扩展名数组长度
    const char** glfwExtensions;                                                            // 实例扩展名数组
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);                // 获取一个包含所有 glfw 所需 Vulkan 实例扩展名的字符串数组
    std::vector<const char*> vExtensions(glfwExtensions,glfwExtensions+glfwExtentionCount); // Vulkan 实例扩展名的字符串数组
    // 如果需要验证层，添加一个验证层的扩展
    if(EnabledValidationLayer){
        vExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
/*
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);                          // 分配一个集合去持有扩展的详细信息
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());    // 查找所有扩展
    // 所有扩展名信息输出
    std::cout << "available extensions:" << std::endl;
    for (const auto& extension : extensions) {
        std::cout << extension.extensionName << std::endl;
    }

    // Vulkan 需要的扩展名信息输出
    std::cout << "glfwExtensions need extensions:" << std::endl;
    // 检查glfwGetRequiredInstanceExtensions返回的所有扩展是否都包含在所有扩展名信息输出的扩展列表中。 
    for(int index = 0;index < glfwExtentionCount;index++){
        std::cout<<*(glfwExtensions + index)<<std::endl;
    }
*/
    VkInstanceCreateInfo createInfo{};                                                       // 指定新创建实例信息的结构体
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;                               // 必填，扩展链类型标识（与pNext成员解析相关联）
    createInfo.pApplicationInfo = &appInfo;                                                  // 连接应用程序信息的结构体
    createInfo.enabledExtensionCount   = vExtensions.size();                                 // 要启用的全局扩展的数量
    createInfo.ppEnabledExtensionNames = vExtensions.data();                                 // 连接实例扩展名数组
    // 如果需要验证层，配置验证层信息
    VkDebugUtilsMessengerCreateInfoEXT createInfoEXT;
    if(EnabledValidationLayer){
        createInfo.enabledLayerCount    = LayerNames.size();                                // 启用的验证层（Validation Layers）的数量
        createInfo.ppEnabledLayerNames  = LayerNames.data();                                // 连接验证层名称的数组
        PopulateDebugUtilsMessengerCreateInfo(createInfoEXT);                               // 设置Debug Messenger的创建信息
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&createInfoEXT;             // 连接Debug Messenger的创建信息 这一步囊括VkDebugUtilsMessengerEXT被创建前的调试信息
    }
    else{
        createInfo.enabledLayerCount    = 0;                                                
        createInfo.ppEnabledLayerNames  = nullptr;                                          
    }
    VkResult results;                                                                       // 返回值枚举信息（几乎所有的 Vulkan 函数都返回 VkResult 类型的值）
    // 尝试创建一个Vulkan核心实例
    if((results = vkCreateInstance(&createInfo,nullptr/*默认Allocation*/,&m_Instance)) != VK_SUCCESS)
    {
        switch (results)
        {
        case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:                                        // 驱动程序不支持Vulkan！
            std::cout<<"Driver didn't support Vulkan!"<<std::endl;
            break;
        case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:                                      // Vulkan无法支持您提供的扩展！
            std::cout<<"Vulkan couldn't support extensions you provided!"<<std::endl;
            break;
        default:// 未知错误！
            std::cout<<"Unknown Error!"<<std::endl;             
            break;
        }
        throw std::runtime_error("Failed to create instance!");                             // 创建实例失败！
    }
}
LcWindow::~LcWindow(){
    CleanupAll();
}
void LcWindow::Run(){
    float lastTime= 0.f;
    while(!glfwWindowShouldClose(m_Window)){
        glfwPollEvents();
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (m_CursorDisabled) {
            m_Camera.ProcessKeyInput(deltaTime);
        }

        DrawFrame();
    }
    vkDeviceWaitIdle(m_LogicDevice);
}
void LcWindow::CleanupWindow(){

    CleanupVulkan();                                                                            // 清除 Vulkan 相关
    glfwDestroyWindow(m_Window);                                                                // 销毁 glfw 窗体实例
    glfwTerminate();                                                                            // 终止 glfw 并释放所有分配的资源
}


void LcWindow::CleanupAll(){
    CleanupWindow();
}

bool LcWindow::CheckValidationLayerSupport() {
    uint32_t availableLayerCount;
    vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);                          // 获取可用的实例层（instance layers）数量
    std::vector<VkLayerProperties> availableLayerProperties(availableLayerCount);
    vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayerProperties.data());  // 获取可用的实例层（instance layers）内容
    // 遍历对比，如果想要启用的验证层有一个找不到就false
    for (const char* LayerName : LayerNames) {
        bool layerFound = false;
        for (const auto& layerProperty : availableLayerProperties) {
            if (std::strcmp(LayerName, layerProperty.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (layerFound == false) {
            return false;
        }
    }
    return true;
}
void LcWindow::CreateLogicDevice(){
    std::set<uint32_t> indices = {m_Family.graphicsQueueFamily.value(),m_Family.presentQueueFamily.value()};
    float queuePriorities = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for(uint32_t index : indices){
        VkDeviceQueueCreateInfo queueCreateInfo{};                                          // 队列创建信息结构体
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;                 // 必填，结构体类型标识
        queueCreateInfo.queueFamilyIndex = index;                                           // 队列族索引
        queueCreateInfo.queueCount = 1;                                                     // 队列数量
        queueCreateInfo.pQueuePriorities = &queuePriorities;                                // 优先级（0.0 ~ 1.0）数值越高，调度时优先级越高
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures features{};                                                    // 物理设备特性结构体，成员照默认值，可修改
    features.samplerAnisotropy  = VK_TRUE;                                                  // 启用异向过滤
    VkDeviceCreateInfo createInfo{};                                                        // 逻辑设备队列创建信息结构体
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;                                // 必填，结构体类型标识
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());       // 要创建的队列族数量
    createInfo.pQueueCreateInfos = queueCreateInfos.data();                                 // 队列创建信息数组
    createInfo.pEnabledFeatures = &features;                                                // 指向启用的物理设备特性结构体
    createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());      // 逻辑设备层面启用的扩展数量
    createInfo.ppEnabledExtensionNames = DeviceExtensions.data();                           // 扩展名数组指针，为 nullptr 表示未启用扩展
    // 尝试根据实际设备创建逻辑设备
    if(vkCreateDevice(m_PhysicalDevice,&createInfo,nullptr,&m_LogicDevice) != VK_SUCCESS){
        throw std::runtime_error("Failed to create logic device!");                         // 创建逻辑设备失败
    }
    
    vkGetDeviceQueue(m_LogicDevice,m_Family.graphicsQueueFamily.value(),0,&m_GraphicsQueue);// 获取图形队列句柄
    vkGetDeviceQueue(m_LogicDevice,m_Family.presentQueueFamily.value(),0,&m_PresentQueue);  // 获取逻辑队列句柄

}

void LcWindow::CreateSurface(){
    // 默认方式创建窗口表面
    VkWin32SurfaceCreateInfoKHR surface_create_info{};                                                  // 用于配置 Win32 平台的 Vulkan 表面创建参数结构体
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;                        // 结构体类型标识，这里一个 Win32 表面创建信息
    surface_create_info.hwnd = glfwGetWin32Window(m_Window);                                            // 窗口句柄
    surface_create_info.hinstance = GetModuleHandle(nullptr);                                           // 当前可执行文件的模块句柄，即程序句柄
    if (vkCreateWin32SurfaceKHR(m_Instance, &surface_create_info, nullptr, &m_Surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to create surface_khr!");                                      // 创建窗口表面失败
    }
    // 使用glfw的创建窗口表面
/*
    if(glfwCreateWindowSurface(m_Instance,m_window,nullptr,&m_Surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to create surface_khr!");                                      // 创建窗口表面失败
    }
*/
    
}

bool LcWindow::CheckDeviceExtensionSupported(VkPhysicalDevice c_device){
    uint32_t deviceExtensionCount;
    // 查询设备支持的扩展数量
    vkEnumerateDeviceExtensionProperties(c_device,nullptr,&deviceExtensionCount,nullptr);
    std::vector<VkExtensionProperties> deviceExtensionsProperties(deviceExtensionCount);
    // 获取设备支持的扩展列表
    vkEnumerateDeviceExtensionProperties(c_device,nullptr,&deviceExtensionCount,deviceExtensionsProperties.data());
    // 把程序需要的扩展放到一个集合中
    std::set<std::string> requiredExtensions(DeviceExtensions.begin(),DeviceExtensions.end());
    // 留下“设备不支持”的扩展
    for(const auto& Extension: deviceExtensionsProperties){
        requiredExtensions.erase(Extension.extensionName);
    }
    // 如果集合为空，表示所有需要的扩展都被支持
    return requiredExtensions.empty();
}
LcSwapChainDetails LcWindow::QuerySwapChainSupport(VkPhysicalDevice c_device){
    LcSwapChainDetails details{};
    details.cap = GetSurfaceCap(c_device);
    details.formats = GetSurfaceFmt(c_device);
    details.modes = GetSurfacePM(c_device);
    return details;
}
VkSurfaceCapabilitiesKHR LcWindow::GetSurfaceCap(VkPhysicalDevice c_device){
    VkSurfaceCapabilitiesKHR SurCap;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(c_device,m_Surface,&SurCap);
    return SurCap;
}
std::vector<VkSurfaceFormatKHR> LcWindow::GetSurfaceFmt(VkPhysicalDevice c_device){
    std::vector<VkSurfaceFormatKHR> SurFmt{};
    uint32_t FmtCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(c_device,m_Surface,&FmtCount,nullptr);
    if(FmtCount !=0 ){
        SurFmt.resize(FmtCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(c_device,m_Surface,&FmtCount,SurFmt.data());
    }
    return SurFmt;
}
std::vector<VkPresentModeKHR> LcWindow::GetSurfacePM(VkPhysicalDevice c_device){
    std::vector<VkPresentModeKHR> PModes{};
    uint32_t PModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(c_device,m_Surface,&PModeCount,nullptr);
    if(PModeCount != 0 ){
        PModes.resize(PModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(c_device,m_Surface,&PModeCount,PModes.data());
    }
    return PModes;
}
VkSurfaceFormatKHR LcWindow::ChooseSurfaceFmt(const LcSwapChainDetails& details){
    const std::vector<VkSurfaceFormatKHR>& availableFormats = details.formats;
    for (const auto& availableFormat : availableFormats) {
        // 找到希望的颜色格式
        if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    // 没找到就使用第一个
    return availableFormats[0];
}
VkPresentModeKHR LcWindow::ChoosePresentMode(const LcSwapChainDetails& details){
    const std::vector<VkPresentModeKHR>& availablePresentModes = details.modes;
    for(const auto& availablePresentModes: availablePresentModes) {
        // 这里希望找到三缓冲作为呈现模式
        if(availablePresentModes == VK_PRESENT_MODE_MAILBOX_KHR){
            return availablePresentModes;
        }
    }
    // 没找到就返回双缓冲
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D LcWindow::ChooseExtent2D(const LcSwapChainDetails& details){
    const VkSurfaceCapabilitiesKHR& capabilities = details.cap;
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
        return capabilities.currentExtent;
    }
    // 相等情况可以支持自定义选择
    else{
        int width, height;
        glfwGetFramebufferSize(m_Window, &width,&height);// 获取实际窗口范围（交换链图像的分辨率）
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}

void LcWindow::CreateSwapChain(){
    LcSwapChainDetails details = QuerySwapChainSupport(m_PhysicalDevice);   // 获取交换链细节
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFmt(details);           // 获取需要的表面能力
    VkPresentModeKHR presentMode = ChoosePresentMode(details);              // 获取需要的呈现模式
    VkExtent2D extent = ChooseExtent2D(details);                            // 获取需要的交换范围（交换链图像分辨率）

    // 我们将尝试大于1的图像数量，以实现三重缓冲
    uint32_t imageCount = details.cap.minImageCount + 1;
    if (details.cap.minImageCount > 0 && imageCount > details.cap.maxImageCount) {
        imageCount = details.cap.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};                          // 交换链创建信息结构体
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; // 必填，结构体类型标识
    createInfo.surface = m_Surface;                                 // 要绑定的窗口表面
    createInfo.minImageCount = imageCount;                          // 图像数量
    createInfo.imageFormat = surfaceFormat.format;                  // 颜色格式
    createInfo.imageColorSpace = surfaceFormat.colorSpace;          // 色彩空间
    createInfo.imageExtent = extent;                                // 交换链中图像的分辨率
    createInfo.imageArrayLayers = 1;                                // 通常是 1（多视图渲染才>1）
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;    // 用作颜色附件（Color Attachment）

    // 补充配置部分，主要处理 队列族共享模式 和 其他交换链属性
    LcQueueFamily lqf = m_Family;                                   // 获取之前Find的队列族
    uint32_t queueFamilyIndices[] = {lqf.graphicsQueueFamily.value(), lqf.presentQueueFamily.value()};
    if (lqf.graphicsQueueFamily != lqf.presentQueueFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;   // 不同队列族
        createInfo.queueFamilyIndexCount = 2;                       // 索引数量
        createInfo.pQueueFamilyIndices = queueFamilyIndices;        // 索引列表
    } 
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;    // 相同队列族
        createInfo.queueFamilyIndexCount = 0;                       // 不需要额外指定队列族列表 0
        createInfo.pQueueFamilyIndices = nullptr;                   // 不需要额外指定队列族列表 nullptr
    }
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;          // 图像完全不透明，忽略 Alpha 通道（默认，性能最佳）
    createInfo.preTransform = details.cap.currentTransform;                 // 表面变换
    createInfo.presentMode = presentMode;                                   // 呈现模式设置
    createInfo.clipped = VK_TRUE;                                           // 是否允许 Vulkan 裁剪被其他窗口遮挡的像素（优化渲染性能）
    createInfo.oldSwapchain = VK_NULL_HANDLE;                               // 重建交换链（如窗口大小调整）时，复用旧交换链的资源（如图像内存），暂时不填

    // 尝试创建交换链
    if(vkCreateSwapchainKHR(m_LogicDevice,&createInfo,nullptr,&m_SwapChain) != VK_SUCCESS){
        throw std::runtime_error("Failed to create swap chain!");           // 交换链创建失败！
    }

    m_SwapChainImageFormat = surfaceFormat.format;                          // 交换链像素格式
    m_SwapChainImageExtent = extent;                                        // 交换链交换范围
}

void LcWindow::GetSwapChainImages(std::vector<VkImage>& images) {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(m_LogicDevice, m_SwapChain, &imageCount, nullptr);
    if (imageCount != 0) {
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_LogicDevice, m_SwapChain, &imageCount, images.data());
    }
}
void LcWindow::CreateImageView() {
    GetSwapChainImages(m_SwapChainImages);

    m_SwapChainImageViews.resize(m_SwapChainImages.size());
    for (size_t index = 0; index < m_SwapChainImageViews.size(); index++) {

        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        m_SwapChainImageViews[index] = CreateImageView(m_SwapChainImages[index],format,aspect,1);
    }
}

void LcWindow::CreateRenderPass(){
    VkAttachmentDescription colorAttachmentDescriptionResolve{};
    colorAttachmentDescriptionResolve.flags = 0;
    colorAttachmentDescriptionResolve.format = m_SwapChainImageFormat;
    colorAttachmentDescriptionResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentDescriptionResolve.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescriptionResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescriptionResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescriptionResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescriptionResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescriptionResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachmentDescription{};
    depthAttachmentDescription.flags = 0;
    depthAttachmentDescription.format = FindDepthFormat();
    depthAttachmentDescription.samples = m_MsaaSamples;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.flags = 0;
    colorAttachmentDescription.format = m_SwapChainImageFormat;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentDescription.samples = m_MsaaSamples;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference resolveAttachmentRef{};
    resolveAttachmentRef.attachment = 2;
    resolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.flags = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentRef;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
    subpassDescription.pResolveAttachments = &resolveAttachmentRef;

    std::array<VkAttachmentDescription, 3> attachments = {
        colorAttachmentDescription,
        depthAttachmentDescription,
        colorAttachmentDescriptionResolve,
    };

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependency.dstSubpass = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDescription;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &subpassDependency;
    if (vkCreateRenderPass(m_LogicDevice, &createInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}


void LcWindow::CreateParticlePipeline() {

    std::vector<char> particleVertexCode = ReadFile("./Shader/particleVert.spv");
    std::vector<char> particleFragmentCode = ReadFile("./Shader/particleFrag.spv");

    VkShaderModule particleVertexShaderModule = CreateShaderModule(particleVertexCode);
    VkShaderModule particleFragmentShaderModule = CreateShaderModule(particleFragmentCode);

    VkPipelineShaderStageCreateInfo vertexStageInfo{};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.module = particleVertexShaderModule;
    vertexStageInfo.pName = "main";


    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = particleFragmentShaderModule;
    fragmentStageInfo.pName = "main";

  
    VkPipelineShaderStageCreateInfo ShaderStageInfos[] = {
        vertexStageInfo,
        fragmentStageInfo
    };

    auto bindingDescriptions = LcParticle::getBindingDescriptions(); // 顶点缓冲绑定
    auto attributeDescriptions = LcParticle::getAttributeDescriptions();

    // 固定功能（配置一些参数）
    // 1. 输入汇编器（Input Assembler）：顶点数据到图元的“组装工厂”

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // 输入汇编器（Input Assembler）
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
    assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; // 点作为 GS 输入
    assemblyInfo.primitiveRestartEnable = VK_FALSE;


    // 2. 视口与裁剪矩形（Viewport & Scissor）：渲染区域的“边界框”
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)m_SwapChainImageExtent.width;
    viewport.height = (float)m_SwapChainImageExtent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_SwapChainImageExtent;

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    // 3. 光栅化器（Rasterizer）：三角形到片段的“离散器”
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.depthClampEnable = VK_FALSE;
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth = 1.f;                      // 需物理设备支持 wideLines 特性
    rasterizationInfo.cullMode = VK_CULL_MODE_NONE;         // 剔除模式
    rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;  // 正面的定义（顺时针）
    rasterizationInfo.depthBiasEnable = VK_FALSE;           // 深度偏移
    rasterizationInfo.depthBiasConstantFactor = 0.f;
    rasterizationInfo.depthBiasSlopeFactor = 0.f;
    rasterizationInfo.depthBiasClamp = 0.f;

    // 4. 多重采样（Multisampling）：抗锯齿的“硬件加速器”
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.rasterizationSamples = m_MsaaSamples;   // 多重采样
    multisampleInfo.sampleShadingEnable = VK_FALSE;
    multisampleInfo.minSampleShading = 1.f;
    multisampleInfo.pSampleMask = nullptr;
    multisampleInfo.alphaToCoverageEnable = VK_FALSE;

    // 5. 深度与模板测试 (Depth/Stencil Test)：可见性的“裁判”
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;           // 开启深度测试
    depthStencilInfo.depthWriteEnable = VK_FALSE;          // 写入深度缓冲
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS; // 深度比较方式（越近越优先）
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;    // 不用深度范围测试
    depthStencilInfo.stencilTestEnable = VK_FALSE;        // 暂时不用模板测试

    // 6. 颜色混合 (Color Blending)：像素颜色的“混合器”
/*
    if (blendEnable) {
        finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
        finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
        finalColor = newColor;
    }

    finalColor = finalColor & colorWriteMask;
*/
    VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
    colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentInfo.blendEnable = VK_TRUE;
    colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;                    // 颜色累加混合
    colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;                    // 透明度累加混合
    colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachmentInfo;
    colorBlendInfo.blendConstants[0] = 0.f;
    colorBlendInfo.blendConstants[1] = 0.f;
    colorBlendInfo.blendConstants[2] = 0.f;
    colorBlendInfo.blendConstants[3] = 0.f;

    // 7. 管线布局（VkPipelineLayout）：资源与管线的“接口契约”
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_ParticleUboSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(m_LogicDevice, &pipelineLayoutInfo, nullptr, &m_ParticlePipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo{};                             // 动态状态信息结构体
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicInfo.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = 2;
    createInfo.pStages = ShaderStageInfos;
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &assemblyInfo;
    createInfo.pViewportState = &viewportInfo;
    createInfo.pRasterizationState = &rasterizationInfo;
    createInfo.pMultisampleState = &multisampleInfo;
    createInfo.pDepthStencilState = &depthStencilInfo;
    createInfo.pColorBlendState = &colorBlendInfo;
    createInfo.pDynamicState = &dynamicInfo;
    createInfo.layout = m_ParticlePipelineLayout;
    createInfo.renderPass = m_RenderPass;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;

    std::vector<char> cacheData = ReadFile(ParticlePipelineCachePath);
    VkPipelineCacheCreateInfo cacheInfo{};
    cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    cacheInfo.initialDataSize = cacheData.size();
    cacheInfo.pInitialData = cacheData.data();
    VkPipelineCache cache;

    // 尝试从缓存创建渲染管线
    if (vkCreatePipelineCache(m_LogicDevice, &cacheInfo, nullptr, &cache) == VK_SUCCESS) {
        if (vkCreateGraphicsPipelines(m_LogicDevice, cache, 1, &createInfo, nullptr, &m_ParticlePipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline!");
        }
        // 获取并保存新的缓存数据
        size_t cacheSize_;
        std::vector<char> cacheData_;
        vkGetPipelineCacheData(m_LogicDevice, cache, &cacheSize_,nullptr);
        cacheData_.resize(cacheSize_);
        vkGetPipelineCacheData(m_LogicDevice, cache, &cacheSize_, cacheData_.data());
        WriteFile(ParticlePipelineCachePath, cacheData_, cacheSize_);
        vkDestroyPipelineCache(m_LogicDevice, cache, nullptr);
    } 
    // 从缓存读取失败，重新创建渲染管线
    else {
        if (vkCreateGraphicsPipelines(m_LogicDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_ParticlePipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline!");    // 无法创建渲染管线！
        }
    }

    vkDestroyShaderModule(m_LogicDevice, particleVertexShaderModule, nullptr);
    vkDestroyShaderModule(m_LogicDevice, particleFragmentShaderModule, nullptr);
}

void LcWindow::CreateCubePipeline(){

    std::vector<char> cubeVertexCode = ReadFile("./Shader/cubeVert.spv");
    std::vector<char> cubeFragmentCode = ReadFile("./Shader/cubeFrag.spv");

    VkShaderModule cubeVertexShaderModule = CreateShaderModule(cubeVertexCode);
    VkShaderModule cubeFragmentShaderModule = CreateShaderModule(cubeFragmentCode);

    VkPipelineShaderStageCreateInfo vertexStageInfo{};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.module = cubeVertexShaderModule;
    vertexStageInfo.pName = "main";


    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = cubeFragmentShaderModule;
    fragmentStageInfo.pName = "main";

  
    VkPipelineShaderStageCreateInfo ShaderStageInfos[] = {
        vertexStageInfo,
        fragmentStageInfo
    };

    auto bindingDescriptions = LcVertex::getBindingDescriptions(); // 顶点缓冲绑定
    auto attributeDescriptions = LcVertex::getAttributeDescriptions();

    // 固定功能（配置一些参数）
    // 1. 输入汇编器（Input Assembler）：顶点数据到图元的“组装工厂”

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // 输入汇编器（Input Assembler）
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
    assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // 三角形作为 GS 输入
    assemblyInfo.primitiveRestartEnable = VK_FALSE;


    // 2. 视口与裁剪矩形（Viewport & Scissor）：渲染区域的“边界框”
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)m_SwapChainImageExtent.width;
    viewport.height = (float)m_SwapChainImageExtent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_SwapChainImageExtent;

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    // 3. 光栅化器（Rasterizer）：三角形到片段的“离散器”
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.depthClampEnable = VK_FALSE;
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth = 1.f;                      // 需物理设备支持 wideLines 特性
    rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;    // 剔除模式
    rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;  // 正面的定义（顺时针）
    rasterizationInfo.depthBiasEnable = VK_FALSE;           // 深度偏移
    rasterizationInfo.depthBiasConstantFactor = 0.f;
    rasterizationInfo.depthBiasSlopeFactor = 0.f;
    rasterizationInfo.depthBiasClamp = 0.f;

    // 4. 多重采样（Multisampling）：抗锯齿的“硬件加速器”
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.rasterizationSamples = m_MsaaSamples;   // 多重采样
    multisampleInfo.sampleShadingEnable = VK_FALSE;
    multisampleInfo.minSampleShading = 1.f;
    multisampleInfo.pSampleMask = nullptr;
    multisampleInfo.alphaToCoverageEnable = VK_FALSE;

    // 5. 深度与模板测试 (Depth/Stencil Test)：可见性的“裁判”
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;           // 开启深度测试
    depthStencilInfo.depthWriteEnable = VK_TRUE;          // 写入深度缓冲
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS; // 深度比较方式（越近越优先）
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;    // 不用深度范围测试
    depthStencilInfo.stencilTestEnable = VK_FALSE;        // 暂时不用模板测试

    // 6. 颜色混合 (Color Blending)：像素颜色的“混合器”
/*
    if (blendEnable) {
        finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
        finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
        finalColor = newColor;
    }

    finalColor = finalColor & colorWriteMask;
*/
    VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
    colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentInfo.blendEnable = VK_TRUE;
    colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;                    // 颜色累加混合
    colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;                    // 透明度累加混合
    colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachmentInfo;
    colorBlendInfo.blendConstants[0] = 0.f;
    colorBlendInfo.blendConstants[1] = 0.f;
    colorBlendInfo.blendConstants[2] = 0.f;
    colorBlendInfo.blendConstants[3] = 0.f;

    // 7. 管线布局（VkPipelineLayout）：资源与管线的“接口契约”
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_CubeUboSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(m_LogicDevice, &pipelineLayoutInfo, nullptr, &m_CubePipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo{};                             // 动态状态信息结构体
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicInfo.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = 2;
    createInfo.pStages = ShaderStageInfos;
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &assemblyInfo;
    createInfo.pViewportState = &viewportInfo;
    createInfo.pRasterizationState = &rasterizationInfo;
    createInfo.pMultisampleState = &multisampleInfo;
    createInfo.pDepthStencilState = &depthStencilInfo;
    createInfo.pColorBlendState = &colorBlendInfo;
    createInfo.pDynamicState = &dynamicInfo;
    createInfo.layout = m_CubePipelineLayout;
    createInfo.renderPass = m_RenderPass;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;

    std::vector<char> cacheData = ReadFile(CubePipelineCachePath);
    VkPipelineCacheCreateInfo cacheInfo{};
    cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    cacheInfo.initialDataSize = cacheData.size();
    cacheInfo.pInitialData = cacheData.data();
    VkPipelineCache cache;

    // 尝试从缓存创建渲染管线
    if (vkCreatePipelineCache(m_LogicDevice, &cacheInfo, nullptr, &cache) == VK_SUCCESS) {
        if (vkCreateGraphicsPipelines(m_LogicDevice, cache, 1, &createInfo, nullptr, &m_CubePipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline!");
        }
        // 获取并保存新的缓存数据
        size_t cacheSize_;
        std::vector<char> cacheData_;
        vkGetPipelineCacheData(m_LogicDevice, cache, &cacheSize_,nullptr);
        cacheData_.resize(cacheSize_);
        vkGetPipelineCacheData(m_LogicDevice, cache, &cacheSize_, cacheData_.data());
        WriteFile(CubePipelineCachePath, cacheData_, cacheSize_);
        vkDestroyPipelineCache(m_LogicDevice, cache, nullptr);
    } 
    // 从缓存读取失败，重新创建渲染管线
    else {
        if (vkCreateGraphicsPipelines(m_LogicDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_CubePipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline!");    // 无法创建渲染管线！
        }
    }

    vkDestroyShaderModule(m_LogicDevice, cubeVertexShaderModule, nullptr);
    vkDestroyShaderModule(m_LogicDevice, cubeFragmentShaderModule, nullptr);
}


VkShaderModule LcWindow::CreateShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};                                      // 创建着色器模块的结构体
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;             // 必填，类型标识
    createInfo.codeSize = code.size();                                          // 代码长度
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());          // 代码（转const uint32_t*）
    VkShaderModule shaderModule;                                                // 着色器模块
    VkResult results;
    
    if ((results = vkCreateShaderModule(m_LogicDevice, &createInfo, nullptr, &shaderModule)) != VK_SUCCESS) {
        switch (results) {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                std::cout << "The host is running low on memory!" << std::endl;     // 主机内存不足！
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                std::cout << "The device is running low on memory!" << std::endl;   // 设备内存不足！
                break;
            default:
                std::cout << "Unknown Error!" << std::endl;                         // 未知错误！
                break;
        }
        throw std::runtime_error("Failed to create shader module!");                // 无法创建着色器模块！
    }
    return shaderModule;
}

void LcWindow::CreateFrameBuffers() {
    m_SwapChainFrameBuffers.resize(m_SwapChainImageViews.size());

    for (int index = 0; index < m_SwapChainImageViews.size(); index++) {
        std::array<VkImageView, 3> attachments = {
            m_ColorImageView,
            m_DepthImageView,
            m_SwapChainImageViews[index]
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.width = m_SwapChainImageExtent.width;
        createInfo.height = m_SwapChainImageExtent.height;
        createInfo.layers = 1;
        createInfo.renderPass = m_RenderPass;

        if (vkCreateFramebuffer(m_LogicDevice, &createInfo, nullptr, &m_SwapChainFrameBuffers[index]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create frame buffers!");
        }
    }
}

void LcWindow::CreateCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_Family.graphicsQueueFamily.value();

    if (vkCreateCommandPool(m_LogicDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");                 // 无法创建命令池！
    }
}
void LcWindow::CreateCommandBuffers() {
    m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();
    allocInfo.commandPool = m_CommandPool;

    for(uint32_t index = 0;index < MAX_FRAMES_IN_FLIGHT; index++){
        if (vkAllocateCommandBuffers(m_LogicDevice, &allocInfo, &m_CommandBuffers[index]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");        // 无法创建命令缓冲区！
        }
    }
}

void LcWindow::BeginCommandBuffer(VkCommandBuffer& readyCommandBuffer, VkCommandBufferUsageFlags flags, bool isCreated){
    if (isCreated){
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        if (vkAllocateCommandBuffers(m_LogicDevice, &allocInfo, &readyCommandBuffer)){
            throw std::runtime_error("Failed to allocate command buffer!");
        }
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(readyCommandBuffer, &beginInfo)!= VK_SUCCESS){
        throw std::runtime_error("Couldn't begin command buffer!");
    }
}
void LcWindow::EndCommandBuffer(VkCommandBuffer& readyCommandBuffer, bool isSubmitted){
    if (vkEndCommandBuffer(readyCommandBuffer) != VK_SUCCESS){
        throw std::runtime_error("Couldn't end command buffer!");
    }

    if (isSubmitted){
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &readyCommandBuffer;

        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS){
            throw std::runtime_error("Failed to submit!");
        }

        vkDeviceWaitIdle(m_LogicDevice);
        vkFreeCommandBuffers(m_LogicDevice, m_CommandPool, 1, &readyCommandBuffer);
    }
}

void LcWindow::UpdateUboBuffer(uint32_t currentFrame) {
    glm::vec3 cameraPos = m_Camera.GetPosition();
    glm::vec3 cameraFront = m_Camera.GetFront();
    glm::vec3 cameraUp = m_Camera.GetUp();

    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    {
        // 静态变量保存上一帧时间
        static auto lastTime = std::chrono::high_resolution_clock::now();
        // 当前时间
        auto currentTime = std::chrono::high_resolution_clock::now();
        // 计算本帧耗时（秒）
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        // 实时帧率
        float fps = 1.0f / deltaTime;
        // 输出或使用 FPS
        // std::cout << "FPS: " << fps << std::endl;
        // 更新上一帧时间
        lastTime = currentTime;
        if(!IsZero(fps))
            m_Fps = fps;
    }
    

    // // 更新粒子
    // for (size_t i = 0; i < m_Particles.size(); ++i) {
    //     m_Particles[i].pos.x;//-10~10
    //     m_Particles[i].pos.y;//-10~10
    //     m_Particles[i].pos.z;//-10~10
    //     m_Particles[i].color.r;//0~1
    //     m_Particles[i].color.b;//0~1
    //     m_Particles[i].color.g;//0~1
    //     m_Particles[i].color.a;//0~1
    //     m_Particles[i].size;//0~20
    // }

    // 更新相机
    LcUniformBufferObject UBO;
    UBO.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    UBO.proj = glm::perspective(glm::radians(m_Fov), m_SwapChainImageExtent.width / (float)m_SwapChainImageExtent.height, 0.1f, 100.f);
    UBO.proj[1][1] *= -1;
    memcpy(m_UboData[currentFrame], &UBO, sizeof(UBO));


    for (size_t i = 0; i <m_UseParticleNumber; ++i) {
        // 计算粒子在圆周上的角度
        float angle = (float)i / m_UseParticleNumber * 6.283185f + delta * swirlSpeed; // 2π * i/N + 时间旋转

        // 半径可随时间和粒子编号轻微波动（更自然）
        float r = radius + 1.0f * std::sin(delta * frequency + i * 0.1f);

        // 计算位置：围绕 Y 轴旋转的漩涡
        m_Particles[i].pos.x = std::cos(angle) * r;
        m_Particles[i].pos.z = std::sin(angle) * r;
        m_Particles[i].pos.y = std::sin(delta * heightSpeed + i * 0.05f) * 5.0f;

        // 可选：让颜色或透明度随高度变化
        float t = (m_Particles[i].pos.y + 5.0f) / 10.0f; // 映射到 0~1
        m_Particles[i].color.r = 0.2f + 0.8f * t;
        m_Particles[i].color.g = 0.5f * (1.0f - t);
        m_Particles[i].color.b = 1.0f;
        m_Particles[i].color.a = 1.0f - 0.5f * std::abs(std::sin(delta + i * 0.1f));

        // 可选：让粒子大小有呼吸感
        m_Particles[i].size = 10.0f + 5.0f * std::sin(delta * 2.0f + i * 0.3f);
    }
    std::memcpy(m_ParticleDataMapped,m_Particles.data(),sizeof(LcParticle) * m_UseParticleNumber);

}


void LcWindow::RecordCommandBuffer(VkCommandBuffer recordCommandBuffer, uint32_t imageIndex) {

    BeginCommandBuffer(recordCommandBuffer, 0,false);

    // 只写入一部分粒子
    {
        VkDeviceSize copySize = sizeof(LcParticle) * m_UseParticleNumber;
        memcpy(m_ParticleDataMapped, m_Particles.data(), copySize);
        VkBufferCopy copyRegion{};
        copyRegion.size = copySize;
        vkCmdCopyBuffer(recordCommandBuffer, m_ParticleStagingBuffer, m_ParticleBuffer, 1, &copyRegion);
    }


    std::array<VkClearValue, 3> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 }; 
    clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f }; 

    VkRenderPassBeginInfo passBeginInfo{};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.framebuffer = m_SwapChainFrameBuffers[imageIndex];
    passBeginInfo.clearValueCount = clearValues.size();
    passBeginInfo.pClearValues = clearValues.data();
    passBeginInfo.renderArea.offset = {0, 0};
    passBeginInfo.renderArea.extent = m_SwapChainImageExtent;
    passBeginInfo.renderPass = m_RenderPass;

    // 开始渲染通道
    vkCmdBeginRenderPass(recordCommandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


    if(m_ShowCube)
    {
        vkCmdBindPipeline(recordCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_CubePipeline);

        // 顶点缓冲区（必须在绘制前绑定）
        VkBuffer vertexBuffers[] = { m_VertexBuffer, m_InstanceBuffers[m_CurrentFrame] };
        VkDeviceSize offsets[] = { 0, 0 };
        vkCmdBindVertexBuffers(recordCommandBuffer, 0, 2, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(recordCommandBuffer,m_IndexBuffer,0,VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(recordCommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,m_CubePipelineLayout,0,1,&m_CubeDescriptorSets[m_CurrentFrame],0,nullptr);

        // 设置视口
        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float) m_SwapChainImageExtent.width;
        viewport.height = (float) m_SwapChainImageExtent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(recordCommandBuffer, 0, 1, &viewport);
        // 设置裁剪矩形
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChainImageExtent;
        vkCmdSetScissor(recordCommandBuffer, 0, 1, &scissor);

        vkCmdDrawIndexed(recordCommandBuffer, static_cast<uint32_t>(m_Indices.size()), static_cast<uint32_t>(m_InstanceDatas.size()), 0, 0, 0);
    }


    {
        // 绑定图形管线
        vkCmdBindPipeline(recordCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ParticlePipeline);
        VkBuffer vertexBuffers[] = { m_ParticleBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(recordCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindDescriptorSets(recordCommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,m_ParticlePipelineLayout,0,1,&m_ParticleDescriptorSets[m_CurrentFrame],0,nullptr);
        // 设置视口
        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float) m_SwapChainImageExtent.width;
        viewport.height = (float) m_SwapChainImageExtent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(recordCommandBuffer, 0, 1, &viewport);
        // 设置裁剪矩形
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChainImageExtent;
        vkCmdSetScissor(recordCommandBuffer, 0, 1, &scissor);
        vkCmdDraw(recordCommandBuffer, m_UseParticleNumber, 1, 0, 0);
    }



    if(m_ShowImgui){

        // 生成Imgui的new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (m_CursorDisabled) {
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse; 
            io.ConfigFlags |= ImGuiConfigFlags_NoKeyboard;
        } else {
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse; 
            io.ConfigFlags &= ~ImGuiConfigFlags_NoKeyboard;
        }

        // Imgui开始生成
        static bool show_demo_window=true;
        if(show_demo_window){
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        static float f = 0.0f;
        static int counter = 0;
        static bool show_editer_window=true;
        ImGui::Begin("Hello, world!"); 
        ImGui::Text("This is some useful text."); 
        ImGui::Checkbox("Demo Window", &show_demo_window); 
        ImGui::Checkbox("Another Window", &show_editer_window);
        ImGui::End();


        if(show_editer_window){
            
            ImGui::Begin("Editer");
            ImGui::DragFloat3("Camera Position", &m_Camera.GetPositionRef()[0], 0.1f);
            ImGui::DragFloat3("Camera Front", &m_Camera.GetFrontRef()[0], 0.1f);
            ImGui::DragFloat3("Camera Up", &m_Camera.GetUpRef()[0], 0.1f);
            ImGui::Checkbox("Show Cube",&m_ShowCube);
            if(ImGui::Button("Init Camera Look")){
                m_Camera.GetPositionRef() = m_Camera.LOOK_POS;
                m_Camera.GetFrontRef() = m_Camera.LOOK_FRONT;
                m_Camera.GetUpRef() = m_Camera.LOOK_UP;

            }


            // 粒子数量
            ImGui::SliderInt("ParticleNumber", &m_UseParticleNumber, 1, MAX_PARTICLES);


            ImGui::Text("Fps: %.3f", m_Fps); 
            // 用滑块控制旋转速度
            ImGui::SliderFloat("Swirl Speed", &swirlSpeed, 0.0f, 10.0f);

            // 用滑块控制半径
            ImGui::SliderFloat("Radius", &radius, 0.0f, 20.0f);

            // 用滑块控制高度变化速度
            ImGui::SliderFloat("Height Speed", &heightSpeed, -5.0f, 5.0f);

            // 用滑块控制波动频率
            ImGui::SliderFloat("Frequency", &frequency, 0.0f, 10.0f);

            ImGui::End();
        }
        

        // Imgui结束生成
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), recordCommandBuffer);
    }

    // 结束渲染通道
    vkCmdEndRenderPass(recordCommandBuffer);
    // 结束命令缓冲区录制
    EndCommandBuffer(recordCommandBuffer,false);
}

void LcWindow::CreateSyncObjects(){
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; 
    m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    for(uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT ; index++){
        if (vkCreateSemaphore(m_LogicDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[index]) != VK_SUCCESS ||
            vkCreateFence(m_LogicDevice, &fenceInfo, nullptr, &m_InFlightFences[index]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create frame sync objects!");
        }
    }
    m_RenderFinishedSemaphores.resize(m_SwapChainImages.size());
    m_ImageInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE); 
    
    for(size_t index = 0; index < m_SwapChainImages.size(); index++){
        if (vkCreateSemaphore(m_LogicDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[index]) != VK_SUCCESS) {
             throw std::runtime_error("Failed to create image render finished semaphores!");
        }
    }
}

void LcWindow::DrawFrame() {

    vkWaitForFences(m_LogicDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
    UpdateUboBuffer(m_CurrentFrame);
    uint32_t imageIndex;
    VkResult results = vkAcquireNextImageKHR(
        m_LogicDevice, m_SwapChain, UINT64_MAX, 
        m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, 
        &imageIndex);
    if(results == VK_ERROR_OUT_OF_DATE_KHR || results == VK_SUBOPTIMAL_KHR){
        RecreateSwapChain();
        return ;
    }
    else if(results != VK_SUCCESS){
        throw std::runtime_error("Failed to acquired images!");    // 无法获取图像！
    }
    if (m_ImageInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(m_LogicDevice, 1, &m_ImageInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        m_ImageInFlight[imageIndex] = VK_NULL_HANDLE; 
    }
    m_ImageInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];
    vkResetFences(m_LogicDevice, 1, &m_InFlightFences[m_CurrentFrame]);
    vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
    RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
    
    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[imageIndex]}; 
    
    VkPipelineStageFlags waitPipelineStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitPipelineStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit queue!");
    }

    VkSwapchainKHR swapChains[] = {m_SwapChain};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores; 
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;
    results = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
    
    if (results == VK_ERROR_OUT_OF_DATE_KHR || results == VK_SUBOPTIMAL_KHR || m_FramebufferResized){
        RecreateSwapChain();
        m_FramebufferResized = false;
    }
    else if (results != VK_SUCCESS){
        throw std::runtime_error("Failed to present images!");
    }
    
    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void LcWindow::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                        VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_LogicDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_LogicDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryTypes(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_LogicDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory!");
    }

    vkBindBufferMemory(m_LogicDevice, buffer, bufferMemory, 0);
}

void LcWindow::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer commandBufferOneTime;
    if (vkAllocateCommandBuffers(m_LogicDevice, &allocInfo, &commandBufferOneTime) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    BeginCommandBuffer(commandBufferOneTime, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,false);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    vkCmdCopyBuffer(commandBufferOneTime, srcBuffer, dstBuffer, 1, &copyRegion);

    EndCommandBuffer(commandBufferOneTime,false);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBufferOneTime;

    vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, nullptr);
    vkQueueWaitIdle(m_GraphicsQueue);

    vkFreeCommandBuffers(m_LogicDevice, m_CommandPool, 1, &commandBufferOneTime);
}


void LcWindow::CreateImage(VkImage& image, VkDeviceMemory& imageMemory, VkFormat format, VkImageType imageType, VkExtent3D extent, VkImageUsageFlags usage, uint32_t mipLevels, VkSampleCountFlagBits sampleCount) {

    if (extent.width == 0 || extent.height == 0 || extent.depth == 0)
        throw std::runtime_error("Invalid image extent: all dimensions must be > 0!");
        
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.format = format;
    createInfo.imageType = imageType;
    createInfo.extent = extent;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.mipLevels = mipLevels;
    createInfo.arrayLayers = 1; 
    createInfo.samples = sampleCount;

    if (vkCreateImage(m_LogicDevice, &createInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_LogicDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryTypes(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_LogicDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory!");
    }

    vkBindImageMemory(m_LogicDevice, image, imageMemory, 0);
}
void LcWindow::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,float mipmapLevels) {
    VkCommandBuffer commandBufferOneTime;
    BeginCommandBuffer(commandBufferOneTime, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,true);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = mipmapLevels;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags srcStage, dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0; // VK_ACCESS_HOST_WRITE_BIT
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(commandBufferOneTime, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    EndCommandBuffer(commandBufferOneTime,true);
}

VkFormat LcWindow::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);

        if ((tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) ||
            (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)) {
            return format;
        }
    }

    throw std::runtime_error("Couldn't find this format!");
}
void LcWindow::CleanupSwapChain() {

    if(m_ColorImageView!=VK_NULL_HANDLE){
        vkDestroyImageView(m_LogicDevice, m_ColorImageView, nullptr);
        m_ColorImageView = VK_NULL_HANDLE;
    }
    if(m_ColorImage!=VK_NULL_HANDLE){
        vkDestroyImage(m_LogicDevice, m_ColorImage, nullptr);
        m_ColorImage = VK_NULL_HANDLE;
    }
    if(m_ColorImageMemory!=VK_NULL_HANDLE){
        vkFreeMemory(m_LogicDevice, m_ColorImageMemory, nullptr);
        m_ColorImageMemory = VK_NULL_HANDLE;
    }

    if(m_DepthImageView!=VK_NULL_HANDLE){
        vkDestroyImageView(m_LogicDevice, m_DepthImageView, nullptr);
        m_DepthImageView = VK_NULL_HANDLE;
    }
    if(m_DepthImage!=VK_NULL_HANDLE){
        vkDestroyImage(m_LogicDevice, m_DepthImage, nullptr);
        m_DepthImage = VK_NULL_HANDLE;
    }
    if(m_DepthImageMemory!=VK_NULL_HANDLE)
        vkFreeMemory(m_LogicDevice, m_DepthImageMemory, nullptr);
    for (auto framebuffer : m_SwapChainFrameBuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(m_LogicDevice, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }
    for (auto imageView : m_SwapChainImageViews) {
        if(imageView!=VK_NULL_HANDLE){
            vkDestroyImageView(m_LogicDevice, imageView, nullptr);
            imageView = VK_NULL_HANDLE;
        }
    }
    if(m_SwapChain!=VK_NULL_HANDLE){
        vkDestroySwapchainKHR(m_LogicDevice, m_SwapChain, nullptr);
        m_SwapChain = VK_NULL_HANDLE;
    }

}

VkFormat LcWindow::FindDepthFormat() {
    return FindSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool LcWindow::HasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void LcWindow::CreateDepthResources() {
    VkFormat format = FindDepthFormat();
    VkImageAspectFlags aspect = HasStencilComponent(format) ? 
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : 
        VK_IMAGE_ASPECT_DEPTH_BIT;
    CreateImage(m_DepthImage, m_DepthImageMemory, format, VK_IMAGE_TYPE_2D, 
        {m_SwapChainImageExtent.width, m_SwapChainImageExtent.height, 1},
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, 
        m_MsaaSamples);
    m_DepthImageView = CreateImageView(m_DepthImage, format, aspect,1);

}

VkImageView LcWindow::CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags,uint32_t baseMipLevel) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.format = format;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.layerCount = 1;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageView imageView;
    if (vkCreateImageView(m_LogicDevice, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view!");
    }

    return imageView;
}

void LcWindow::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBufferOneTime;
    BeginCommandBuffer(commandBufferOneTime, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,true);

    VkBufferImageCopy region{};
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.bufferOffset = 0;
    region.imageExtent = {width, height, 1};
    region.imageOffset = {0, 0, 0};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageSubresource.mipLevel = 0;

    vkCmdCopyBufferToImage(commandBufferOneTime, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    EndCommandBuffer(commandBufferOneTime,true);
}

void LcWindow::CreateTextureImage(){
    int tex_width, tex_height, tex_channles;
    stbi_uc* pixels = stbi_load(TexturePath.c_str(), &tex_width, &tex_height, &tex_channles, STBI_rgb_alpha);
    VkDeviceSize texSize = tex_width * tex_height * 4;
    m_MipmapLevels = std::floor(std::log2(std::max(tex_width,tex_height)))+1;
    if (!pixels) {
        throw std::runtime_error("Failed to load texture!");
    }
    VkImageUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(texSize, usage, properties, stagingBuffer, stagingMemory);
    void* data;
    vkMapMemory(m_LogicDevice, stagingMemory, 0, texSize, 0, &data);
    memcpy(data, pixels, texSize);
    vkUnmapMemory(m_LogicDevice, stagingMemory);
    stbi_image_free(pixels);
    CreateImage(
        m_TextureImage, 
        m_TextureImageMemory, 
        VK_FORMAT_R8G8B8A8_UNORM, 
        VK_IMAGE_TYPE_2D, 
        {(uint32_t) tex_width, (uint32_t) tex_height, 1},
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        m_MipmapLevels, 
        VK_SAMPLE_COUNT_1_BIT
    );
    TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,m_MipmapLevels);
    CopyBufferToImage(stagingBuffer, m_TextureImage, (uint32_t) tex_width, (uint32_t) tex_height);
    vkDestroyBuffer(m_LogicDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicDevice, stagingMemory, nullptr);
    GenerateMipmaps(m_TextureImage,VK_FORMAT_R8G8B8A8_UNORM,(int32_t)tex_width,(int32_t)tex_height,m_MipmapLevels);
    

}

void LcWindow::CreateTextureImageView() {
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    m_TextureImageView = CreateImageView(m_TextureImage, format,aspect,m_MipmapLevels);
}

void LcWindow::CreateTextureSampler() {
    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    // 1. 过滤方式（放大/缩小处理）
    createInfo.magFilter = VK_FILTER_LINEAR; // 处理“放大”场景（纹素少、片段多）
    createInfo.minFilter = VK_FILTER_LINEAR; // 处理“缩小”场景（纹素多、片段少）
    // 2. 寻址模式（坐标超出[0,1]时的行为）
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    // 3. 各向异性过滤（提升斜视角清晰度，解决斜视角下纹理的模糊问题）
    createInfo.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);
    createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    // 4. 边框颜色（仅当寻址模式为CLAMP_TO_BORDER时生效）
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    // 5. 坐标归一化（是否使用[0,1]范围坐标）
    createInfo.unnormalizedCoordinates = VK_FALSE;
    // 6. 比较模式（暂不启用，用于阴影贴图）
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    // 7. Mipmap 设置 (暂不启用)
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // 指定 mipmap 层级间的过滤方式
    createInfo.mipLodBias = 0.f; // 指定 mipmap 层级的偏移量 (如 +1.0 会优先使用更模糊的层级, -1.0 优先使用更清晰的层级)
    createInfo.minLod = 0.f;
    createInfo.maxLod = 0.f;
    if (vkCreateSampler(m_LogicDevice, &createInfo, nullptr, &m_TextureSampler) != VK_SUCCESS){
        throw std::runtime_error("Failed to create sampler!");
    }
}

void LcWindow::GenerateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer oneTimeCommandBuffer;
    BeginCommandBuffer(oneTimeCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, true);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;


    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;
    for (uint32_t index = 1; index < mipLevels; index++) {
        barrier.subresourceRange.baseMipLevel = index - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(oneTimeCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = index - 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = index;

        vkCmdBlitImage(oneTimeCommandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(oneTimeCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr, 0, nullptr, 1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(oneTimeCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndCommandBuffer(oneTimeCommandBuffer, true);
}

void LcWindow::CreateVertexBuffer(){
    VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
    VkBufferUsageFlags stagingUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags stagingProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    CreateBuffer(bufferSize, stagingUsage, stagingProperties, stagingBuffer, stagingMemory);

    void* data;
    vkMapMemory(m_LogicDevice, stagingMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_Vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(m_LogicDevice, stagingMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexMemory);

    CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_LogicDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicDevice, stagingMemory, nullptr);
}
void LcWindow::CreateIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();
    VkBufferUsageFlags stagingUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(bufferSize, stagingUsage, properties, stagingBuffer, stagingMemory);

    void* data;
    vkMapMemory(m_LogicDevice, stagingMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_Indices.data(), bufferSize);
    vkUnmapMemory(m_LogicDevice, stagingMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexMemory);

    CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(m_LogicDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_LogicDevice, stagingMemory, nullptr);
}

uint32_t LcWindow::FindMemoryTypes(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

    for (uint32_t index = 0; index < memProperties.memoryTypeCount; index++) {
        if ((typeFilter & (1 << index)) && (memProperties.memoryTypes[index].propertyFlags & properties) == properties) {
            return index;
        }
    }

    throw std::runtime_error("Couldn't find suitable memory types!");
}

void LcWindow::RecreateSwapChain(){
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Window, &width, &height);
    while (width == 0 || height == 0){
        glfwGetFramebufferSize(m_Window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_LogicDevice);

    CleanupSwapChain();

    CreateSwapChain();
    CreateImageView();
    CreateColorResources();
    CreateDepthResources();
    CreateFrameBuffers();
}

void LcWindow::CreateUboBuffers() {
    VkDeviceSize size = sizeof(LcUniformBufferObject);
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    m_UboBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_UboMemories.resize(MAX_FRAMES_IN_FLIGHT);
    m_UboData.resize(MAX_FRAMES_IN_FLIGHT);

    for (uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT; index++) {
        CreateBuffer(size, usage, properties, m_UboBuffers[index], m_UboMemories[index]);
        vkMapMemory(m_LogicDevice, m_UboMemories[index], 0, size, 0, &m_UboData[index]);
    }
}



void LcWindow::CreateCubeDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        uboLayoutBinding,
        samplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
    uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    uboLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    uboLayoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_LogicDevice, &uboLayoutInfo, nullptr, &m_CubeUboSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create ubo descriptor set layout!");
    }
}

void LcWindow::CreateParticleDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {
        uboLayoutBinding 
    };

    VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
    uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    uboLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    uboLayoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_LogicDevice, &uboLayoutInfo, nullptr, &m_ParticleUboSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create ubo descriptor set layout!");
    }
}

void LcWindow::CreateCubeDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSize{};
    poolSize[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    poolSize[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;
    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
    poolInfo.pPoolSizes = poolSize.data();
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
    poolInfo.flags = 0;

    if (vkCreateDescriptorPool(m_LogicDevice, &poolInfo, nullptr, &m_CubeDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void LcWindow::CreateParticleDescriptorPool() {
    std::array<VkDescriptorPoolSize, 1> poolSize{};
    poolSize[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size()); 
    poolInfo.pPoolSizes = poolSize.data();
    
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT; 
    poolInfo.flags = 0;

    if (vkCreateDescriptorPool(m_LogicDevice, &poolInfo, nullptr, &m_ParticleDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void LcWindow::CreateCubeDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_CubeUboSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.descriptorPool = m_CubeDescriptorPool;
    allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;

    m_CubeDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_LogicDevice, &allocInfo, m_CubeDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT; index++) {
        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = m_UboBuffers[index];
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = sizeof(LcUniformBufferObject);

        std::array<VkWriteDescriptorSet, 2> descriptorWrite{};
        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = m_CubeDescriptorSets[index];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pBufferInfo = &descriptorBufferInfo;

        // 组合图像采样器的“资源打包器”
        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = m_TextureImageView;
        descriptorImageInfo.sampler = m_TextureSampler;

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].dstSet = m_CubeDescriptorSets[index];
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pImageInfo = &descriptorImageInfo;

        vkUpdateDescriptorSets(m_LogicDevice, descriptorWrite.size(), descriptorWrite.data(), 0, nullptr);
    }
}

void LcWindow::CreateParticleDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_ParticleUboSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.descriptorPool = m_ParticleDescriptorPool;
    allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;

    m_ParticleDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_LogicDevice, &allocInfo, m_ParticleDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (uint32_t index = 0; index < MAX_FRAMES_IN_FLIGHT; index++) {
        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = m_UboBuffers[index];
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = sizeof(LcUniformBufferObject);
        std::array<VkWriteDescriptorSet, 1> descriptorWrite{};
        
        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = m_ParticleDescriptorSets[index];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pBufferInfo = &descriptorBufferInfo;

        vkUpdateDescriptorSets(m_LogicDevice, descriptorWrite.size(), descriptorWrite.data(), 0, nullptr);
    }
}

void LcWindow::CreateLcInstanceBuffers() {
    VkDeviceSize bufferSize = sizeof(m_InstanceDatas[0]) * m_InstanceDatas.size();
    m_InstanceBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_InstanceMemories.resize(MAX_FRAMES_IN_FLIGHT);
    m_InstanceDataMapped.resize(MAX_FRAMES_IN_FLIGHT);
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        CreateBuffer(bufferSize, usage, properties, m_InstanceBuffers[i], m_InstanceMemories[i]);
        vkMapMemory(m_LogicDevice, m_InstanceMemories[i], 0, bufferSize, 0, &m_InstanceDataMapped[i]);
        memcpy(m_InstanceDataMapped[i], m_InstanceDatas.data(), (size_t)bufferSize);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL LcWindow::DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    // 输出验证层信息
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

    // VK_TRUE 表示中断执行（通常不用），VK_FALSE 表示继续执行
    return VK_FALSE;
}


std::vector<char> LcWindow::ReadFile(const std::string& filePath){
    std::ifstream file(filePath,std::ios::ate | std::ios::binary);      // 二进制读文件
    if(!file.is_open()){
        throw std::runtime_error("Failed to open file!(R)");            // 打开文件失败
    }
    size_t fileSize = (size_t) file.tellg();
    file.seekg(0);                                                      // 移动到文件开头
    std::vector<char> fileData(fileSize);
    file.read(fileData.data(),fileSize);
    return fileData;
}

void LcWindow::WriteFile(const std::string& filePath, std::vector<char> Data, size_t dataSize) {
    std::ofstream file(filePath, std::ios::out | std::ios::binary); 
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file!(W)");            // 打开文件失败
    }

    file.write(Data.data(), dataSize);
}

void LcWindow::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (window == nullptr) {
        return;
    }
    auto* app = reinterpret_cast<LcWindow*>(glfwGetWindowUserPointer(window));
    if (app->m_CursorDisabled) {
        app->m_Camera.ProcessMouseInput(xpos, ypos);
    }
}

void LcWindow::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (window == nullptr) {
        return;
    }
    auto* app = reinterpret_cast<LcWindow*>(glfwGetWindowUserPointer(window));
    if (app->m_CursorDisabled) {
        app->m_Fov = app->m_Camera.ProcessMouseZoom(app->m_Fov, xoffset, yoffset);
    }
}

void LcWindow::ToggleCursorMode() {
    m_CursorDisabled = !m_CursorDisabled;
    if (m_CursorDisabled) {
        std::cout<<"[Info] Pos Set Window Cursor"<<std::endl;
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        std::cout<<"[Info] Pos Lost Window Cursor"<<std::endl;
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void LcWindow::ToggleImguiDisplay() {
    m_ShowImgui = !m_ShowImgui;
}
void LcWindow::SetupMessenger(){
    if(EnabledValidationLayer == false) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;                                                      // 调试信使(Debug Messenger)创建信息的结构体
    PopulateDebugUtilsMessengerCreateInfo(createInfo);                                                  // 设置调试信使(Debug Messenger)的创建信息
    // 尝试创建调试程序信使
    if(CreateDebugUtilsMessenger(m_Instance,&createInfo,nullptr,&m_DebugMessenger) != VK_SUCCESS){
        throw std::runtime_error("Failed to create debug utils messenger!");                            // 无法创建调试程序信使
    }
}

void LcWindow::PopulateDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessenger){
    // 清零结构体
    debugUtilsMessenger = {};
    // 必填，设置结构体类型
    debugUtilsMessenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // 配置消息严重级别
    debugUtilsMessenger.messageSeverity =   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |             // 错误
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |           // 警告
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;            // 详细日志
    // 配置消息类型
    debugUtilsMessenger.messageType =   VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |                   // 一般事件
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |               // 性能相关
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;                 // 验证层消息
    // 设置回调函数
    debugUtilsMessenger.pfnUserCallback = LcWindow::DebugCallBack;
    // 可选的用户数据指针                        
    debugUtilsMessenger.pUserData = nullptr;
}

VkResult LcWindow::CreateDebugUtilsMessenger(  VkInstance c_instance,
                                                VkDebugUtilsMessengerCreateInfoEXT* c_createInfo,
                                                VkAllocationCallbacks* c_allocation,
                                                VkDebugUtilsMessengerEXT* c_debugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT  func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(c_instance,"vkCreateDebugUtilsMessengerEXT");
    if(func == nullptr){
        return VK_ERROR_EXTENSION_NOT_PRESENT;                                                          // 请求的扩展在当前环境中不可用
    }
    else{
        return func(c_instance,c_createInfo,c_allocation,c_debugMessenger);
    }
}

void LcWindow::DestroyDebugUtilsMessenger(VkInstance c_instance,VkDebugUtilsMessengerEXT c_debugMessenger,VkAllocationCallbacks* c_allocation)
{
    PFN_vkDestroyDebugUtilsMessengerEXT  func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(c_instance,"vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
        func(c_instance,c_debugMessenger,c_allocation);
}


void LcWindow::PickupPhysicalDevice(){
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(m_Instance,&deviceCount,nullptr);                // 获取物理设备数量
    if(deviceCount == 0){
        throw std::runtime_error("Failed to enumerate physical devices!");      // 没有任何物理设备！
    }
    std::vector<VkPhysicalDevice>devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance,&deviceCount,devices.data());         // 查询 Vulkan 实例中可用的物理设备（如显卡）

    for(const auto& physicalDevice:devices){
        // 判断物理设备是否合适
        if(IsDeviceSuitable(physicalDevice)){
            std::cout<<"[Info] Find physical device good"<<std::endl;
            m_PhysicalDevice = physicalDevice;
            m_MsaaSamples = GetUsableSampleCount();
            std::cout<<"[Info] MsaaSamples "<<m_MsaaSamples<<std::endl;
            break;
        }
        else{
            std::cout<<"[Info] Find physical device useless!"<<std::endl;
        }
    }
    if(m_PhysicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("None of devices is suitable!");               // 没有一个物理设备是合适的！
    }
}


VkSampleCountFlagBits LcWindow::GetUsableSampleCount() {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

    VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts &
                                properties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
    if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
    if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
    if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
    if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
    if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

void LcWindow::CreateColorResources() {
    VkFormat format = m_SwapChainImageFormat;
    uint32_t width = m_SwapChainImageExtent.width;
    uint32_t height = m_SwapChainImageExtent.height;

    CreateImage(m_ColorImage, m_ColorImageMemory, format, VK_IMAGE_TYPE_2D, {width, height, 1},
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                1, m_MsaaSamples);
    m_ColorImageView = CreateImageView(m_ColorImage, format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

bool LcWindow::IsDeviceSuitable(VkPhysicalDevice c_device){
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(c_device,&deviceProperties);                      // 获取物理设备的基本信息
    vkGetPhysicalDeviceFeatures(c_device,&deviceFeatures);                          // 查询物理设备支持的可选功能

    std::cout<<"[Info] Find physical device name: "<<deviceProperties.deviceName<<std::endl;
    m_Family = FindLcQueueFamilyIndex(c_device);                                    // 查找到需要的队列族信息

    bool deviceExtensionSupported = CheckDeviceExtensionSupported(c_device);        // 是否所有需要的扩展是否都被支持
    bool swapChainAdquate = false;                                                  // 交换链细节是否可用
    if(deviceExtensionSupported){
        LcSwapChainDetails details = QuerySwapChainSupport(c_device);               // 查询指定物理设备的交换链支持信息
        swapChainAdquate = !details.formats.empty() && !details.modes.empty();      // 判断交换链细节可用
    }

    // 当前选择物理设备的特性
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&   // 物理设备的类型是否为独立显卡（Discrete GPU）
                                        deviceFeatures.geometryShader &&            // 物理设备是否支持几何着色器（Geometry Shader）功能
                                        deviceFeatures.samplerAnisotropy &&         // 各项异性过滤
                                        m_Family.isComplete() &&                    // 队列存在值
                                        deviceExtensionSupported &&                 // 需要的扩展都被支持
                                        swapChainAdquate                            // 交换链支持信息有效
                                        ; 
}

LcQueueFamily LcWindow::FindLcQueueFamilyIndex(VkPhysicalDevice c_device){
    uint32_t queueFamiliesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(c_device,&queueFamiliesCount,nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
    // 查询 Vulkan 物理设备（VkPhysicalDevice）支持的队列族（Queue Family）属性
    vkGetPhysicalDeviceQueueFamilyProperties(c_device,&queueFamiliesCount,queueFamilies.data()); 

    int index = 0;
    LcQueueFamily foundQueueFamily; 
    for(const auto& QF : queueFamilies){
        // 查找一个支持图形渲染的队列族索引
        if(QF.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            foundQueueFamily.graphicsQueueFamily = index;
            std::cout<<"[Info] Find graphics queue family index: "<<index<<std::endl;
        }
        VkBool32 presentSupported = false;
        // 查找一个支持呈现的队列族索引
        vkGetPhysicalDeviceSurfaceSupportKHR(c_device,index,m_Surface,&presentSupported);
        if(presentSupported == VK_TRUE){
            foundQueueFamily.presentQueueFamily = index;
            std::cout<<"[Info] Find present queue family index: "<<index<<std::endl;
        }
        // 都找到了就返回
        if(foundQueueFamily.isComplete()){
            break;
        }
        index++;
    }
    return foundQueueFamily;
}

void LcWindow::FramebufferResizeCallback(GLFWwindow* window,int width,int height){
    LcWindow* app = reinterpret_cast<LcWindow*>(glfwGetWindowUserPointer(window));
    app->m_FramebufferResized = true;
}

void LcWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    LcWindow* app = reinterpret_cast<LcWindow*>(glfwGetWindowUserPointer(window));
    if (app == nullptr) {
        std::cerr << "错误: 无法获取 LcWindow 实例指针。" << std::endl;
        return;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        app->ToggleCursorMode();
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        app->ToggleImguiDisplay();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, VK_TRUE);
        std::cout<<"[Info] Window Close"<<std::endl;
    }
}