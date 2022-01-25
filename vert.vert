#version 450

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.model * ubo.view * ubo.proj * vec4(position, 1.0);
    fragColor = vec3(0.0,0.2,1.0);
}
