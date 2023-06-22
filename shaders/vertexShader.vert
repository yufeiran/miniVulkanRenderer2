
#version 450
#pragma vscode_glsllint_stage : vert

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout( push_constant ) uniform constants
{
    mat4 model_matrix;
    
}PushConstant;


layout(binding=0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;


void main(){
    gl_Position = ubo.proj * ubo.view * PushConstant.model_matrix * vec4(inPosition,1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;

}