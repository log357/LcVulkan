#include "LcCamera.hpp"
const glm::vec3 LcCamera::POS   = glm::vec3(0.f, 0.f, 2.f);
const glm::vec3 LcCamera::FRONT = glm::vec3(0.f, 0.f, -1.f);
const glm::vec3 LcCamera::UP    = glm::vec3(0.f, 1.f, 0.f);

const glm::vec3 LcCamera::LOOK_POS        = glm::vec3(1.f, -25.f, 15.f);
const glm::vec3 LcCamera::LOOK_FRONT      = glm::vec3(0.f, 1.f, -0.5f);
const glm::vec3 LcCamera::LOOK_UP         = glm::vec3(0.f, 1.f, 0.f);

LcCamera::LcCamera()
:
    LcCamera(LcCamera::LOOK_POS,LcCamera::LOOK_FRONT,LcCamera::LOOK_UP)
{
}
LcCamera::LcCamera(glm::vec3 position, glm::vec3 cameraFront, glm::vec3 cameraUp)
:
    m_CameraPos(position),
    m_CameraFront(cameraFront),
    m_CameraUp(cameraUp),
    m_CameraSpeed(3.f),
    m_ScalingFactor(2.f),
    m_MouseSensitivity(0.05f),
    m_CameraWindow(nullptr),
    m_FirstMouse(true)
{
}
LcCamera::~LcCamera() {
}
void LcCamera::SetProcessingInput(GLFWwindow* window) {
    if (window == nullptr) {
        std::cout << "WINDOW IS NULL HANDLE!";
        return;
    }
    m_CameraWindow = window;
}
void LcCamera::ProcessKeyInput(float delta) {
    if (m_CameraWindow == nullptr) {
        return;
    }
    glm::vec3 currentPos = GetPosition();
    glm::vec3 Front = GetFront();
    glm::vec3 Up = GetUp();
    glm::vec3 Right = glm::normalize(glm::cross(Front, Up));
    float speed = GetSpeed();
    if (glfwGetKey(m_CameraWindow, GLFW_KEY_W) == GLFW_PRESS) {
        currentPos += Front * delta * speed;
        SetPosition(currentPos);
    }
    if (glfwGetKey(m_CameraWindow, GLFW_KEY_S) == GLFW_PRESS) {
        currentPos -= Front * delta * speed;
        SetPosition(currentPos);
    }
    if (glfwGetKey(m_CameraWindow, GLFW_KEY_A) == GLFW_PRESS) {
        currentPos -= Right * delta * speed;
        SetPosition(currentPos);
    }
    if (glfwGetKey(m_CameraWindow, GLFW_KEY_D) == GLFW_PRESS){
        currentPos += (Right * delta * speed);
        SetPosition(currentPos);
    }
    if (glfwGetKey(m_CameraWindow, GLFW_KEY_SPACE) == GLFW_PRESS){
        currentPos += (Up * delta * speed);
        SetPosition(currentPos);
    }
    if (glfwGetKey(m_CameraWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        currentPos -= (Up * delta * speed);
        SetPosition(currentPos);
    }

}

void LcCamera::ProcessMouseInput(double xpos, double ypos) {
    static double lastX = 0.0;
    static double lastY = 0.0;
    static float yaw   = -90.0f;
    static float pitch = 0.0f;
    if (m_FirstMouse) {
        lastX = xpos;
        lastY = ypos;
        yaw = -90.0f;
        pitch = 0.0f;
        m_FirstMouse = false;
    }

    // 计算偏移量
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // 反转：y向上移动时应抬头

    lastX = xpos;
    lastY = ypos;

    // 灵敏度调整
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    yaw   += static_cast<float>(xoffset);
    pitch += static_cast<float>(yoffset);

    // 限制 pitch，防止“倒头”
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // 计算新的方向向量
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    m_CameraFront = glm::normalize(front);

    // 输出调试信息（可选）
    // std::cout << "yaw: " << yaw << " pitch: " << pitch << std::endl;
    // std::cout << "Front: " << m_CameraFront.x << ", " << m_CameraFront.y << ", " << m_CameraFront.z << std::endl;
}


float LcCamera::ProcessMouseZoom(float z_fov, double xoffset, double yoffset) {
    if (z_fov >= 1.f && z_fov <= 45.f) {
        z_fov -= yoffset;
    }
    if (z_fov <= 1.f) {
        z_fov = 1.f;
    }
    if (z_fov >= 45.f) {
        z_fov = 45.f;
    }
    return z_fov;
}
