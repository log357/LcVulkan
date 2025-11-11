#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 3) in vec4 inModelRow0;
layout(location = 4) in vec4 inModelRow1;
layout(location = 5) in vec4 inModelRow2;
layout(location = 6) in vec4 inModelRow3;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform UBO {
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    mat4 model = mat4(inModelRow0, inModelRow1, inModelRow2, inModelRow3);
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
