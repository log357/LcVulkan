#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in float inSize;
layout(location = 2) in vec4 inColor;

layout(binding = 0) uniform UBO {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec4 vColor;

void main()
{
    vec4 clipPos = ubo.proj * ubo.view * vec4(inPos, 1.0);
    gl_Position = clipPos;
    gl_PointSize = inSize;
    vColor = inColor;
}