#version 450 



layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;


layout(set = 0, binding = 0) uniform sampler2D ssaoSampler;



void main()
{
    vec2 texelSize = 1.0 / textureSize(ssaoSampler, 0);
    float result = 0.0;
    for(int x = -2; x < 2; ++x)
    {
        for(int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoSampler, TexCoords + offset).r;
        }
    }
    result /= 16.0;
    FragColor = vec4(vec3(result), 1.0);

}