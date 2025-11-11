#pragma once
#include "LcStuff.hpp"

class LcCamera {
public:
    static const glm::vec3 POS;
    static const glm::vec3 FRONT;
    static const glm::vec3 UP;

    static const glm::vec3 LOOK_POS;
    static const glm::vec3 LOOK_FRONT;
    static const glm::vec3 LOOK_UP;

public:
    LcCamera();
    LcCamera(glm::vec3 position,glm::vec3 cameraFront,glm::vec3 cameraUp);
    ~LcCamera();
public:
    inline glm::vec3    GetPosition() { return m_CameraPos; };
    inline glm::vec3    GetFront() { return m_CameraFront; };
    inline glm::vec3    GetUp() { return m_CameraUp; };
    inline glm::vec3&   GetPositionRef() { return m_CameraPos; };
    inline glm::vec3&   GetFrontRef() { return m_CameraFront; };
    inline glm::vec3&   GetUpRef() { return m_CameraUp; };
    inline float        GetSpeed() { return m_CameraSpeed; };
    inline float        GetscalingFactor() { return m_ScalingFactor; };
    inline float        GetSensitivity() { return m_MouseSensitivity; };
    inline void         SetPosition(glm::vec3 Pos) { m_CameraPos = Pos; };
    inline void         SetFront(glm::vec3 Front) { m_CameraFront = Front; };
    inline void         SetUp(glm::vec3 Up) { m_CameraUp = Up; };
    inline void         SetSpeed(float Speed) { m_CameraSpeed = Speed; };
    inline void         SetScalingFactor(float Factor) { m_ScalingFactor = Factor; };
    inline void         SetSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; };
    void                SetProcessingInput(GLFWwindow* window);
    void                ProcessKeyInput(float delta);
    void                ProcessMouseInput(double xpos, double ypos);
    float               ProcessMouseZoom(float z_fov, double xoffset, double yoffset);
private:
    glm::vec3           m_CameraPos;
    glm::vec3           m_CameraFront;
    glm::vec3           m_CameraUp;
    float               m_CameraSpeed;
    float               m_ScalingFactor;
    float               m_MouseSensitivity;
    GLFWwindow*         m_CameraWindow;
    bool                m_FirstMouse;

};