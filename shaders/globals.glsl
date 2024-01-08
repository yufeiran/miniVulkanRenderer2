// Use to save all constant, gloabl values and structures not shared with CPP

#ifndef GLOBALS_GLSL
#define GLOBALS_GLSL 1

#define PI 3.1415926535897323
#define TWO_PI 6.28318530717958648
#define INFINITY 1e32
#define EPS 0.0001

const float M_PI        = 3.14159265358979323846;   // pi
const float M_TWO_PI    = 6.28318530717958648;      // 2*pi
const float M_PI_2      = 1.57079632679489661923;   // pi/2
const float M_PI_4      = 0.785398163397448309616;  // pi/4
const float M_1_OVER_PI = 0.318309886183790671538;  // 1/pi
const float M_2_OVER_PI = 0.636619772367581343076;  // 2/pi

const float LIGHT_CONSTANT = 1.0f;
const float LIGHT_LINEAR    = 0.7f;
const float LIGHT_QUADRATIC = 1.8f;

struct Ray 
{
    vec3 origin;
    vec3 direction;
};

struct hitPayload 
{
    uint seed;
    float hitT;
    int   primitiveID;
    int   instanceID;
    int   instanceCustomIndex;
    vec2  baryCoord;
    mat4x3 objectToWorld;
    mat4x3 worldToObject;

};

struct Material
{
    vec3   albedo;
    float  specular;
    vec3   emission;
    float  anisotropy;
    float  metallic;
    float  roughness;
    float  subsurface;
    float  specularTint;
    float  sheen;
    vec3   sheenTint;
    float  clearcoat;
    float  clearcoatRoughness;
    float  transmission;
    float  ior;
    vec3   attenuationColor;
    float  attenuationDistance;

    // Roughness calculated from anisotropic
    float  ax;
    float  ay;
    // ---
    vec3   f0;
    float  alpha;
    bool   unlit;
    bool   thinwalled;
};

struct State 
{
    int    depth;
    float  eta;

    vec3   position;
    vec3   normal;
    vec3   ffnormal;
    vec3   tangent;
    vec3   bitangent;
    vec2   texCoord;

    bool   isEmitter;
    bool   specularBounce;
    bool   isSubsurface;

    uint     matID;
    Material mat;

};

struct BsdfSampleRec
{
    vec3  L;
    vec3  f;
    float pdf;
};



#endif //GLOBALS_GLSL