// 用来存同步在GPU内存和CPU内存的结构,比如uniform结构
#ifndef COMMON_DEVICE_DATA_STRUCT
#define COMMON_DEVICE_DATA_STRUCT

#ifdef __cplusplus
#include"../src/Common/common.h"
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using uint = unsigned int;
#endif

#ifdef __cplusplus
	#define START_BINDING(a) enum a{ 
	#define END_BINDING() }
#else 
	#define START_BINDING(a) const uint
	#define END_BINDING() 
#endif

START_BINDING(SceneBindings)
	eGlobals  = 0,
	eObjDescs = 1,
	eTextures = 2,
	eCubeMap  = 3
END_BINDING();

START_BINDING(RtBindings)
	eTlas     = 0,  // Top-level acceleration structure
	eOutImage = 1
END_BINDING();



struct ObjDesc
{
	int txtOffset;
	uint64_t vertexAddress;
	uint64_t indexAddress;
	uint64_t materialAddress;
	uint64_t materialIndexAddress;
};

struct GlobalUniforms
{
	mat4 viewProj;
	mat4 viewInverse;
	mat4 projInverse;
};

struct Vertex
{
	vec3 pos;
	vec3 normal;
	vec3 color;
	vec2 texCoord;
};

struct PushConstantRaster
{
	mat4  modelMatrix;
	vec3  lightPosition;
	uint  objIndex;
	float lightIntensity;
	int   lightType;
};

struct PushConstantRay
{
	vec4  clearColor;
	vec3  lightPosition;
	float lightIntensity;
	int   lightType;
	int   frame;
	int   nbSample;
};

struct WaveFrontMaterial
{
	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	vec3  transmittance;
	vec3  emission;
	float shininess;
	float ior;
	float dissolve;
	int   illum;
	int   textureId;
};

// for raytracing


#endif