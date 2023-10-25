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
	eGlobals = 0,
	eObjDescs = 1,
	eTextures = 2
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
};

struct GlobalUniforms
{
	mat4 viewProj;
	mat4 viewInverse;
	mat4 projInverse;
};

struct PushConstantRaster
{
	mat4 modelMatrix;
};


// for raytracing


#endif