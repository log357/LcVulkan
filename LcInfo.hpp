#pragma once
#include "LcStuff.hpp"
// 渲染管线缓存路径
const std::string ParticlePipelineCachePath = "./Cache/particlePipelineConfig.config";
const std::string CubePipelineCachePath = "./Cache/cubePipelineConfig.config";


// 纹理图像路径
const std::string TexturePath = "./Textures/Liberty-MattoniBasamento-1.bmp";

// 模型路径
const std::string ModelPath = "./Models/untitled.obj";

// 决定关闭/开启验证层
// #define NDEBUG
#ifdef NDEBUG
    const bool EnabledValidationLayer = false; 
#else
    const bool EnabledValidationLayer = true; 
#endif

// 最大帧
const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

// 启用的验证层列表（验证层需要通过指定其名称来启用）
const std::vector<const char*> LayerNames = {
    "VK_LAYER_KHRONOS_validation"
};
// 启用的交换链设备扩展列表
const std::vector<const char*> DeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
// 窗体信息结构体
struct LcWindowInfo{
    int width;
    int height;
    std::string title;
};
// 队列族信息结构体
struct LcQueueFamily{
    std::optional<uint32_t> graphicsQueueFamily;    // 图形队列族
    std::optional<uint32_t> presentQueueFamily;     // 呈现队列族
    bool isComplete(){
        return  graphicsQueueFamily.has_value() && 
                presentQueueFamily.has_value();
    }
};

// 相机矩阵结构体
struct LcUniformBufferObject{
    glm::mat4 view;         // 视图矩阵
    glm::mat4 proj;         // 投影矩阵
};

// 交换链细节结构体
struct LcSwapChainDetails{
    VkSurfaceCapabilitiesKHR                cap;     // 表面能力
    std::vector<VkSurfaceFormatKHR>         formats; // 支持的图像格式
    std::vector<VkPresentModeKHR>           modes;   // 呈现模式
};

// 物体实例变换结构体
struct LcInstanceData {
    alignas(16) glm::mat4 modelMatrix; 
};
// 物体顶点信息结构体
struct LcVertex{
    glm::vec3   inPosition;
    glm::vec3   inColor;
    glm::vec2   inTexCoord;
    LcVertex() = default;
    LcVertex( glm::vec3 ip, glm::vec3 ic, glm::vec2 it):
        inPosition(ip),
        inColor(ic),
        inTexCoord(it)
    {
    }
    LcVertex(const LcVertex& lv)
    {
        inPosition = lv.inPosition;
        inColor = lv.inColor;
        inTexCoord = lv.inTexCoord;
    }
    bool operator==(const LcVertex& other)const {
        return inPosition == other.inPosition && inColor == other.inColor && inTexCoord==other.inTexCoord;
    }
    static std::array<VkVertexInputBindingDescription, 2> getBindingDescriptions() {
        std::array<VkVertexInputBindingDescription, 2> bindings{};
        // 顶点缓冲 binding 0
        bindings[0].binding = 0;
        bindings[0].stride = sizeof(LcVertex);
        bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        // 实例缓冲 binding 1
        bindings[1].binding = 1;
        bindings[1].stride = sizeof(glm::mat4); // 每个实例一个 mat4
        bindings[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        return bindings;
    }
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        // 顶点属性
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LcVertex, inPosition)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LcVertex, inColor)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(LcVertex, inTexCoord)});
        // 实例矩阵属性，拆成 4 个 vec4
        for (uint32_t i = 0; i < 4; i++) {
            VkVertexInputAttributeDescription instanceAttrib{};
            instanceAttrib.binding = 1; // 实例缓冲
            instanceAttrib.location = 3 + i;
            instanceAttrib.format = VK_FORMAT_R32G32B32A32_SFLOAT;
            instanceAttrib.offset = sizeof(glm::vec4) * i;
            attributeDescriptions.push_back(instanceAttrib);
        }
        return attributeDescriptions;
    }
};

namespace std {
    template<> struct hash<LcVertex> {
        size_t operator()(const LcVertex& vertex) const {
            return ((hash<glm::vec3>()(vertex.inPosition) ^
                    (hash<glm::vec3>()(vertex.inColor) << 1)) >> 1) ^
                    (hash<glm::vec2>()(vertex.inTexCoord) << 1);
        }
    };
}

// 粒子顶点信息结构体
struct LcParticle {
    glm::vec3   pos;
    float       size;
    glm::vec4   color;
    static std::array<VkVertexInputBindingDescription, 1> getBindingDescriptions() {
        std::array<VkVertexInputBindingDescription, 1> bindings{};
        bindings[0].binding = 0;
        bindings[0].stride = sizeof(LcParticle);
        bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindings;
    }
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LcParticle, pos)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32_SFLOAT, offsetof(LcParticle, size)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(LcParticle, color)});

        return attributeDescriptions;
    }
};
// 粒子池最大10w个
const uint32_t MAX_PARTICLES = 100000;