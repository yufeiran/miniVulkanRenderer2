#include "deviceDataStruct.h"

vec3 computeDiffuse(Material mat, vec3 lightDir, vec3 normal)
{
    // Lambertian 
    float dotNL = max(dot(normal, lightDir), 0.0);
    vec3  c     = mat.diffuse * dotNL;
    if(mat.illum >= 1)
        c += mat.ambient;
    return c;
}

vec3 computeSpecular(Material mat, vec3 viewDir, vec3 lightDir, vec3 normal)
{
    if(mat.illum < 2)
        return vec3(0);
        
}