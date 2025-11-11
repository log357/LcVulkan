#pragma once
#include "LcStuff.hpp"
#include "LcCamera.hpp"
#include "LcImgui.hpp"
#include "LcInfo.hpp"

class LcWindow{
public:
    LcWindow();
    LcWindow(LcWindowInfo window_info);
    ~LcWindow();
public:
    void                                    Run();                                                                                                                                                                                                  // 运行
    void                                    ToggleCursorMode();                                                                                                                                                                                     // 切换窗体焦点状态
    void                                    ToggleImguiDisplay();                                                                                                                                                                                   // 切换Imgui显示隐藏
private:
    VkResult                                CreateDebugUtilsMessenger(VkInstance c_instance,VkDebugUtilsMessengerCreateInfoEXT* c_createInfo,VkAllocationCallbacks* c_allocation,VkDebugUtilsMessengerEXT* c_debugMessenger);                       // 创建调试信使
    void                                    DestroyDebugUtilsMessenger(VkInstance c_instance,VkDebugUtilsMessengerEXT c_debugMessenger,VkAllocationCallbacks* c_allocation);                                                                        // 销毁调试信使
    static VKAPI_ATTR VkBool32 VKAPI_CALL   DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,VkDebugUtilsMessageTypeFlagsEXT  messageTypes,const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,void* pUserData);          // 验证信息回调
    static void                             FramebufferResizeCallback(GLFWwindow* window,int width,int height);                                                                                                                                     // 窗口尺寸变换回调
    static std::vector<char>                ReadFile(const std::string& filePath);                                                                                                                                                                  // 读文件
    static void                             WriteFile(const std::string& filePath, std::vector<char> Data, size_t dataSize);                                                                                                                        // 写文件
    static void                             MouseCallback(GLFWwindow* window, double xpos, double ypos);                                                                                                                                            // 鼠标回调
    static void                             ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);                                                                                                                                     // 滚动回调
    static void                             KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);                                                                                                                           // 按键回调
private:        
    void                                    InitWindow();                                                                                                                                                                                           // 初始化窗体内容
    void                                    InitVulkan();                                                                                                                                                                                           // 初始化Vulkan内容
    void                                    PopulateDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessenger);                                                                                                         // 设置调试信使的创建信息
    void                                    InitCamera();                                                                                                                                                                                           // 初始化相机
    void                                    CreateInstance();                                                                                                                                                                                       // 创建实例 
    void                                    CleanupWindow();                                                                                                                                                                                        // 清除窗体相关
    void                                    CleanupVulkan();                                                                                                                                                                                        // 清除Vulkan相关
    void                                    CleanupAll();                                                                                                                                                                                           // 清除所有内容
    bool                                    CheckValidationLayerSupport();                                                                                                                                                                          // 检测验证层是否被支持
    void                                    SetupMessenger();                                                                                                                                                                                       // 创建调试层
    void                                    PickupPhysicalDevice();                                                                                                                                                                                 // 挑选一个物理设备
    bool                                    IsDeviceSuitable(VkPhysicalDevice c_device);                                                                                                                                                            // 判断物理设备是否合适
    LcQueueFamily                           FindLcQueueFamilyIndex(VkPhysicalDevice c_device);                                                                                                                                                      // 找指定物理设备的队列族索引
    void                                    CreateLogicDevice();                                                                                                                                                                                    // 根据物理设备创建逻辑设备和图形队列
    void                                    CreateSurface();                                                                                                                                                                                        // 创建窗口表面
    bool                                    CheckDeviceExtensionSupported(VkPhysicalDevice c_device);                                                                                                                                               // 检查需要的交换链扩展是否全部都在 GPU 支持的扩展列表中
    LcSwapChainDetails                      QuerySwapChainSupport(VkPhysicalDevice c_device);                                                                                                                                                       // 查询指定物理设备的交换链支持信息
    VkSurfaceCapabilitiesKHR                GetSurfaceCap(VkPhysicalDevice c_device);                                                                                                                                                               // 获取指定物理设备的表面能力
    std::vector<VkSurfaceFormatKHR>         GetSurfaceFmt(VkPhysicalDevice c_device);                                                                                                                                                               // 获取指定物理设备支持的表面格式列表
    std::vector<VkPresentModeKHR>           GetSurfacePM(VkPhysicalDevice c_device);                                                                                                                                                                // 获取指定物理设备支持的呈现模式列表
    VkSurfaceFormatKHR                      ChooseSurfaceFmt(const LcSwapChainDetails& details);                                                                                                                                                    // 获取需要的表面能力
    VkPresentModeKHR                        ChoosePresentMode(const LcSwapChainDetails& details);                                                                                                                                                   // 获取需要的呈现模式
    VkExtent2D                              ChooseExtent2D(const LcSwapChainDetails& details);                                                                                                                                                      // 获取需要的交换范围（交换链图像分辨率）
    void                                    CreateSwapChain();                                                                                                                                                                                      // 创建交换链
    void                                    GetSwapChainImages(std::vector<VkImage>& images);                                                                                                                                                       // 获取交换链图像
    void                                    CreateImageView();                                                                                                                                                                                      // 创建交换链图像视图
    void                                    CreateRenderPass();                                                                                                                                                                                     // 渲染通道
    void                                    CreateParticlePipeline();                                                                                                                                                                               // 创建粒子渲染管线
    void                                    CreateCubePipeline();                                                                                                                                                                                   // 创建方块渲染管线
    VkShaderModule                          CreateShaderModule(const std::vector<char>& code);                                                                                                                                                      // 着色器模块创建函数
    void                                    CreateFrameBuffers();                                                                                                                                                                                   // 创建帧缓冲区
    void                                    CreateCommandPool();                                                                                                                                                                                    // 创建命令池
    void                                    CreateCommandBuffers();                                                                                                                                                                                 // 创建命令缓冲区
    void                                    CreateSyncObjects();                                                                                                                                                                                    // 创建信号量
    void                                    BeginCommandBuffer(VkCommandBuffer& readyCommandBuffer, VkCommandBufferUsageFlags flags,bool isCreated);                                                                                                // 用于开始记录一个命令缓冲区
    void                                    EndCommandBuffer(VkCommandBuffer& readyCommandBuffer,bool isSubmited);                                                                                                                                  // 用于结束记录一个命令缓冲区
    void                                    RecordCommandBuffer(VkCommandBuffer recordCommandBuffer, uint32_t imageIndex);                                                                                                                          // 录制命令缓冲区，执行完整的渲染流程
    void                                    DrawFrame();                                                                                                                                                                                            // 主绘制
    void                                    CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,VkBuffer& buffer, VkDeviceMemory& bufferMemory);                                                             // 创建缓冲区       
    void                                    CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);                                                                                                                                  // 拷贝缓冲区
    void                                    CreateVertexBuffer();                                                                                                                                                                                   // 创建顶点缓冲区
    void                                    CreateIndexBuffer();                                                                                                                                                                                    // 创建索引缓冲区
    uint32_t                                FindMemoryTypes(uint32_t typeFillter,VkMemoryPropertyFlags properties);                                                                                                                                 // 物理寻址
    void                                    RecreateSwapChain();                                                                                                                                                                                    // 重建交换链
    void                                    CreateParticleDescriptorSetLayout();                                                                                                                                                                    // 创建粒子描述符集布局
    void                                    CreateCubeDescriptorSetLayout();                                                                                                                                                                        // 创建方块描述符集布局
    void                                    CreateParticleDescriptorPool();                                                                                                                                                                         // 创建粒子描述符池
    void                                    CreateCubeDescriptorPool();                                                                                                                                                                             // 创建方块描述符池
    void                                    CreateParticleDescriptorSets();                                                                                                                                                                         // 创建粒子描述符集
    void                                    CreateCubeDescriptorSets();                                                                                                                                                                             // 创建方块描述符集
    void                                    CreateUboBuffers();                                                                                                                                                                                     // 创建统一缓冲区对象
    void                                    UpdateUboBuffer(uint32_t currentFrame);                                                                                                                                                                 // 坐标变换
    void                                    TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,float mipmapLevels);                                                                             // 同步图像资源
    void                                    CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);                                                                                                                     // 创建图像缓冲
    void                                    CreateImage(VkImage& image, VkDeviceMemory& imageMemory, VkFormat format, VkImageType imageType, VkExtent3D extent, VkImageUsageFlags usage, uint32_t mipLevels, VkSampleCountFlagBits sampleCount);    // 创建图像
    VkImageView                             CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags,uint32_t baseMipLevel);                                                                                                 // 创建图像视图
    void                                    CreateTextureImage();                                                                                                                                                                                   // 创建图像纹理
    void                                    CreateTextureImageView();                                                                                                                                                                               // 创建纹理图像视图
    void                                    CreateTextureSampler();                                                                                                                                                                                 // 创建纹理采样器
    void                                    CreateDepthResources();                                                                                                                                                                                 // 创建深度缓冲区 
    VkFormat                                FindDepthFormat();                                                                                                                                                                                      // 查找深度格式
    bool                                    HasStencilComponent(VkFormat format);                                                                                                                                                                   // 检查给定的格式是否包含模板分量
    VkFormat                                FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);                                                                                      // 查找物理设备支持的第一个满足指定特性的格式
    void                                    CleanupSwapChain();                                                                                                                                                                                     // 清除交换链
    void                                    LoadModel();                                                                                                                                                                                            // 加载模型
    void                                    GenerateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);                                                                                               // 映射级别
    VkSampleCountFlagBits                   GetUsableSampleCount();                                                                                                                                                                                 // 获取采样信息
    void                                    CreateColorResources();                                                                                                                                                                                 // 创建多重采样资源
    void                                    InitImgui();                                                                                                                                                                                            // 初始化Imgui
    void                                    CreateLcInstanceBuffers();
    void                                    CreateParticleBuffers();

private:
    LcWindowInfo                            m_Info;                             // 窗体信息（自动销毁）
    LcQueueFamily                           m_Family;                           // 队列族的索引（自动销毁）
    GLFWwindow*                             m_Window;                           // glfw窗体指针（手动销毁）
    VkInstance                              m_Instance;                         // 核心对象（手动销毁）
    VkDebugUtilsMessengerEXT                m_DebugMessenger;                   // 检验层信使（手动销毁）
    VkPhysicalDevice                        m_PhysicalDevice;                   // 物理设备句柄（随VkInstance销毁）
    VkDevice                                m_LogicDevice;                      // 逻辑设备（手动销毁）
    VkQueue                                 m_GraphicsQueue;                    // 图形队列（随VkDevice销毁）
    VkQueue                                 m_PresentQueue;                     // 呈现队列（随VkDevice销毁）
    VkSurfaceKHR                            m_Surface;                          // 窗口表面（手动销毁）
    VkSwapchainKHR                          m_SwapChain;                        // 交换链（手动销毁）
    VkFormat                                m_SwapChainImageFormat;             // 交换链像素格式（自动销毁）
    VkExtent2D                              m_SwapChainImageExtent;             // 交换链交换范围（自动销毁）
    std::vector<VkImage>                    m_SwapChainImages;                  // 交换链图像（随VkSwapchainKHR销毁）
    std::vector<VkImageView>                m_SwapChainImageViews;              // 交换链图像视图（手动销毁）
    VkPipelineLayout                        m_ParticlePipelineLayout;           // 粒子管线布局（手动销毁）
    VkPipelineLayout                        m_CubePipelineLayout;               // 方块管线布局（手动销毁）
    VkRenderPass                            m_RenderPass;                       // 渲染通道（手动销毁）
    VkPipeline                              m_ParticlePipeline;                 // 粒子渲染管线（手动销毁）
    VkPipeline                              m_CubePipeline;                     // 方块渲染管线（手动销毁）
    std::vector<VkFramebuffer>              m_SwapChainFrameBuffers;            // 帧缓冲区（手动销毁）
    VkCommandPool                           m_CommandPool;                      // 命令池（手动销毁）
    std::vector<VkCommandBuffer>            m_CommandBuffers;                   // 命令缓冲区（随VkCommandPool销毁）
    std::vector<VkSemaphore>                m_ImageAvailableSemaphores;         // 图像可用信号量（手动销毁）
    std::vector<VkSemaphore>                m_RenderFinishedSemaphores;         // 渲染完成信号量（手动销毁）
    std::vector<VkFence>                    m_InFlightFences;                   // 飞行中栅栏（手动销毁）
    std::vector<VkFence>                    m_ImageInFlight;                    // 图像帧映射栅栏（自动销毁）
    bool                                    m_FramebufferResized;               // 窗口大小改变布尔值（自动销毁）
    uint32_t                                m_CurrentFrame;                     // 信号量索引（自动销毁）
    VkBuffer                                m_VertexBuffer;                     // 顶点缓冲区（手动销毁）
    VkDeviceMemory                          m_VertexMemory;                     // 顶点物理内存信息（手动销毁）
    VkBuffer                                m_IndexBuffer;                      // 索引缓冲区（手动销毁）
    VkDeviceMemory                          m_IndexMemory;                      // 索引物理内存信息（手动销毁）
    VkDescriptorSetLayout                   m_ParticleUboSetLayout;             // 粒子描述符集布局（手动销毁）
    VkDescriptorSetLayout                   m_CubeUboSetLayout;                 // 方块描述符集布局（手动销毁）
    VkDescriptorPool                        m_ParticleDescriptorPool;           // 粒子描述符池（手动销毁）
    VkDescriptorPool                        m_CubeDescriptorPool;               // 方块描述符池（手动销毁）
    std::vector<VkDescriptorSet>            m_ParticleDescriptorSets;           // 粒子描述符集（随VkDescriptorPool销毁）
    std::vector<VkDescriptorSet>            m_CubeDescriptorSets;               // 方块描述符集（随VkDescriptorPool销毁）
    std::vector<VkBuffer>                   m_UboBuffers;                       // UBO缓存（手动销毁）
    std::vector<VkDeviceMemory>             m_UboMemories;                      // UBO关联的设备内存（手动销毁）
    std::vector<void*>                      m_UboData;                          // UBO主机端映射的指针（自动销毁）
    VkImage                                 m_TextureImage;                     // 纹理图像（手动销毁）
    VkDeviceMemory                          m_TextureImageMemory;               // 纹理图像关联的设备内存（手动销毁）
    VkImageView                             m_TextureImageView;                 // 纹理视图（手动销毁）
    VkSampler                               m_TextureSampler;                   // 采样器（手动销毁）
    VkImage                                 m_DepthImage;                       // 深度图像（手动销毁）
    VkDeviceMemory                          m_DepthImageMemory;                 // 深度图像物理内存信息（手动销毁）
    VkImageView                             m_DepthImageView;                   // 深度图像视图（手动销毁）
    LcCamera                                m_Camera;                           // 相机（自动销毁）
    float                                   m_Fov;                              // 视角量（自动销毁）
    std::vector<LcVertex>                   m_Vertices;                         // 顶点信息（自动销毁）
    std::vector<uint32_t>                   m_Indices;                          // 索引信息（自动销毁）
    uint32_t                                m_MipmapLevels;                     // 映射级别（自动销毁）
    VkSampleCountFlagBits                   m_MsaaSamples;                      // 多重采样枚举值（自动销毁）
    VkImage                                 m_ColorImage;                       // 多重采样图像（手动销毁）
    VkDeviceMemory                          m_ColorImageMemory;                 // 多重采样关联的设备内存（手动销毁）
    VkImageView                             m_ColorImageView;                   // 多重采样图像视图（手动销毁）
    VkDescriptorPool                        m_ImguiPool;                        // ImGui描述符池（手动销毁）
    bool                                    m_CursorDisabled;                   // 默认启动时禁用光标（自动销毁）
    bool                                    m_ShowImgui;                        // 显示Imgui（自动销毁）
    bool                                    m_ShowCube;                         // 显示Cube（自动销毁）

    std::vector<LcInstanceData>             m_InstanceDatas;        //（自动销毁）
    std::vector<VkBuffer>                   m_InstanceBuffers;      //（手动销毁）
    std::vector<VkDeviceMemory>             m_InstanceMemories;     //（手动销毁）
    std::vector<void*>                      m_InstanceDataMapped;   //（自动销毁）

    std::vector<LcParticle>                 m_Particles;            //（自动销毁）
    VkBuffer                                m_ParticleBuffer;       //（手动销毁）
    VkDeviceMemory                          m_ParticleMemory;       //（手动销毁）
    VkBuffer                                m_ParticleStagingBuffer;//（手动销毁）
    VkDeviceMemory                          m_ParticleStagingMemory;//（手动销毁）
    void*                                   m_ParticleDataMapped;   //（自动销毁）
    int                                     m_UseParticleNumber;    //（自动销毁）
    float                                   m_Fps;

    // // 控制漩涡参数
    float swirlSpeed = 1.5f;   // 旋转速度
    float radius = 5.0f;       // 漩涡半径
    float heightSpeed = 0.5f;  // 向上（或向下）移动速度
    float frequency = 2.0f;    // 波动频率（让半径轻微变化）

};
