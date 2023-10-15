#version 450
#pragma vscode_glsllint_stage : vert

layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D offscreenColor;

void main()
{
    vec3 color = texture(offscreenColor,TexCoords).rgb;

    FragColor = vec4(color,1.0);
}