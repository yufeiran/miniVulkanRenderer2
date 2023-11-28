#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


// Dummy 
void main()
{

    // vec3 cubeMapColor = texture(cubeMapTexture,gl_WorldRayDirectionEXT).xyz;
    // //prd.hitValue = pcRay.clearColor.xyz * 0.8;
    // if(prd.depth == 0)
    // {
    //     prd.hitValue =  0.8 * cubeMapColor;
    // }
    // else {
    //     prd.hitValue = 0.01 * cubeMapColor; // Tiny contribution from environment;
    // }
    // prd.depth = 100;
}