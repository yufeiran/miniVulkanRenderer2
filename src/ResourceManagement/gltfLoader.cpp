
#include "gltfLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "ResourceManagement/boundingBox.h"
#include <glm/gtc/type_ptr.hpp>


template <class T, class Y>
struct TypeCast
{
	Y operator()(T value) const noexcept
	{
		return static_cast<Y>(value);
	}
};


using namespace mini;
void GltfLoader::loadScene(const std::string& filename, glm::mat4 transform)
{
	reset();


	tinygltf::TinyGLTF tcontext;
	std::string        warn, error;
	Log("Loading file:" + filename);
	if(!tcontext.LoadASCIIFromFile(&tmodel, &error, &warn, filename))
	{
		assert(!"Error while loading scene");
	}
	Log(warn);
	Log(error);
	
	importMaterials(tmodel);

	importDrawableNodes(tmodel, transform, GltfAttributes::Normal | GltfAttributes::Texcoord_0 | GltfAttributes::Color_0 | GltfAttributes::Tangent );


}

void GltfLoader::importMaterials(const tinygltf::Model& tmodel)
{
	materials.reserve(tmodel.materials.size());

	for(auto& tmat : tmodel.materials)
	{
		GltfMaterial gmat;
		gmat.tmaterial = &tmat; // Reference

		gmat.alphaCutoff              = static_cast<float>(tmat.alphaCutoff);
		gmat.alphaMode                = tmat.alphaMode == "MASK" ? 1 : (tmat.alphaMode == "BLEND" ? 2 : 0);
		gmat.doubleSided              = tmat.doubleSided ? 1 : 0;
		gmat.emissiveFactor           = tmat.emissiveFactor.size() == 3 ?
									       glm::vec3(tmat.emissiveFactor[0], tmat.emissiveFactor[1], tmat.emissiveFactor[2]) :
									       glm::vec3(0.f);

		gmat.emissiveTexture          = tmat.emissiveTexture.index;
		gmat.normalTexture            = tmat.normalTexture.index;
		gmat.normalTextureScale       = static_cast<float>(tmat.normalTexture.scale);
		gmat.occlusionTexture         = tmat.occlusionTexture.index;
		gmat.occlusionTextureStrength = static_cast<float>(tmat.occlusionTexture.strength);

		// PbrMetallicRoughness
		auto& tpbr = tmat.pbrMetallicRoughness;
		gmat.baseColorFactor  = 
			glm::vec4(tpbr.baseColorFactor[0], tpbr.baseColorFactor[1], tpbr.baseColorFactor[2], tpbr.baseColorFactor[3]);
		gmat.baseColorTexture         = tpbr.baseColorTexture.index;
		gmat.metallicFactor           = static_cast<float>(tpbr.metallicFactor);
		gmat.metallicRoughnessTexture = tpbr.metallicRoughnessTexture.index;
		gmat.roughnessFactor          = static_cast<float>(tpbr.roughnessFactor);

		gmat.occlusionTexture         = tmat.occlusionTexture.index;
		gmat.occlusionTextureStrength = tmat.occlusionTexture.strength;

		
		gmat.transmissionFactor = 0;
		gmat.transmissionTexture = -1;
		// KHR_materials_transmission
		if(tmat.extensions.find(KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME)!=tmat.extensions.end())
		{
			const auto& ext = tmat.extensions.find(KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME)->second;
			getFloat(ext, "transmissionFactor", gmat.transmissionFactor);
			getTexId(ext, "transmissionTexture", gmat.transmissionTexture);
			
		}
		else {
			gmat.transmissionFactor = 0;
			gmat.transmissionTexture = -1;
		}

		
		//TODO: extension

		materials.emplace_back(gmat);


	}

	// Make default mat
	if(materials.empty())
	{
		GltfMaterial gmat;
		gmat.metallicFactor = 0;
		materials.emplace_back(gmat);
	}
}

float sqrtNorm(const glm::vec3& n)
{
	return n.x * n.x + n.y * n.y + n.z * n.z;
}


// Linearize the scene graph to world space nodes
void GltfLoader::importDrawableNodes(const tinygltf::Model& tmodel, glm::mat4 transform, GltfAttributes requestedAttributes, GltfAttributes forceRequested)
{
	checkRequiredExtensions(tmodel);

	int          defaultScene = tmodel.defaultScene > -1 ? tmodel.defaultScene : 0;
	const auto&  tscene       = tmodel.scenes[defaultScene];

	// Finding used mesh
	std::set<uint32_t> usedMeshes;
	for(auto nodeIdx : tscene.nodes)
	{
		findUsedMeshes(tmodel, usedMeshes, nodeIdx);
	}

	// Find the number of vertex(attributes) and index

	uint32_t nbIndex{0};
	uint32_t primCnt{0};
	for(const auto& m: usedMeshes)
	{
		auto&                 tmesh = tmodel.meshes[m];
		std::vector<uint32_t> vprim;
		for(const auto& primitive : tmesh.primitives)
		{
			if(primitive.mode != 4) // Triangle
				continue;
			const auto& posAccessor = tmodel.accessors[primitive.attributes.find("POSITION")->second];

			if(primitive.indices > -1)
			{
				const auto& indexAccessor = tmodel.accessors[primitive.indices];
				nbIndex += static_cast<uint32_t>(indexAccessor.count);
			}
			else 
			{
				nbIndex += static_cast<uint32_t>(posAccessor.count);
			}
			vprim.emplace_back(primCnt++);
		}
		meshToPrimMeshes[m] = std::move(vprim);  
	}

	// Reserving memory
	indices.reserve(nbIndex);

	// Convert all mesh/primitives+ to a single primitive per mesh
	for(const auto& m : usedMeshes)
	{
		auto& tmesh  = tmodel.meshes[m];
		for(const auto& tprimitive : tmesh.primitives)
		{
			processMesh(tmodel, tprimitive, requestedAttributes, forceRequested, tmesh.name);
			primMeshes.back().tmesh = &tmesh;
			primMeshes.back().tprim = &tprimitive;
		}
	}

	// Fixing tangent
	for(size_t i = 0; i < tangents.size(); i++)
	{
		auto& t =tangents[i];
		if(sqrtNorm(t) < 0.01f 
			//|| std::abs(t.w) < 0.5f
			)
		{
			const auto& n     = normals[i];
			const float sgn   = n.z > 0.0f ? 1.0f : -1.0f;
			const float a     = -1.0f / (sgn + n.z);
			const float b     = n.x * n.y * a;
			t                 = glm::vec4(1.0f + sgn * n.x * n.x * a, sgn * b, -sgn * n.x, sgn);
		}
	}

	// Transforming the scene hierarchy to a flat list
	for(auto nodeIdx : tscene.nodes)
	{
		processNode(tmodel, nodeIdx, transform);
	}

	computeSceneDimensions();
	computeCamera();

	meshToPrimMeshes.clear();
	primitiveIndices32u.clear();
	primitiveIndices16u.clear();
	primitiveIndices8u.clear();
}

void GltfLoader::reset()
{
	materials.clear();   // Material for shading
	nodes.clear();       // Drawable nodes, flat hierarchy
	primMeshes.clear();  // Primitive promoted to meshes
	cameras.clear();
	lights.clear();

	// Attributes, all same length if valid
	positions.clear();
	indices.clear();
	normals.clear();
	tangents.clear();
	texcoords0.clear();
	texcoords1.clear();
	colors0.clear();
	cachePrimMesh.clear();

	tmodel = tinygltf::Model();

	meshToPrimMeshes.clear();
	primitiveIndices32u.clear();
	primitiveIndices16u.clear();
	primitiveIndices8u.clear();
}

void GltfLoader::processNode(const tinygltf::Model& tmodel, int& nodeIdx, const glm::mat4& parentMatrix)
{
	const auto& tnode = tmodel.nodes[nodeIdx];

	glm::mat4 matrix      = getLocalMatrix(tnode);
	glm::mat4 worldMatrix = parentMatrix * matrix;

	if(tnode.mesh > -1)
	{
		const auto& meshes = meshToPrimMeshes[tnode.mesh];  // A mesh could have many primitives
		for(const auto& mesh : meshes)
		{
			GltfNode node;
			node.primMesh     = mesh;
			node.worldMatrix  = worldMatrix;
			node.tnode        = &tnode;
			nodes.emplace_back(node);
		}
	}
	else if(tnode.camera > -1)
	{
		GltfCamera camera;
		camera.worldMatrix = worldMatrix;
		camera.cam         = tmodel.cameras[tmodel.nodes[nodeIdx].camera];

		// TODO: support Iray extension
		cameras.emplace_back(camera);
	}
	else if(tnode.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME) != tnode.extensions.end())
	{
		GltfLight    light;
		const auto&  ext       = tnode.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME)->second;
		auto         lightIdx  = ext.Get("light").GetNumberAsInt();
		light.light            = tmodel.lights[lightIdx];
		light.worldMatrix      = worldMatrix;
		lights.emplace_back(light);
	}

	// Recursion
	for(auto child : tnode.children)
	{
		processNode(tmodel, child, worldMatrix);
	}
}

// Extracting the values to a linear buffer
void GltfLoader::processMesh(const tinygltf::Model& tmodel, const tinygltf::Primitive& tmesh, GltfAttributes requestedAttributes, GltfAttributes forceRequested, const std::string& name)
{
	// Only triangles are supported
	// 0:point, 1:lines, 2:line_loop, 3:line_strip, 4:triangles, 5:triangle_strip, 6:triangle_fan
	if(tmesh.mode != 4)
		return;

	GltfPrimMesh resultMesh;
	resultMesh.name          = name;
	resultMesh.materialIndex = std::max(0, tmesh.material);
	resultMesh.vertexOffset   = static_cast<uint32_t>(positions.size());
	resultMesh.firstIndex    = static_cast<uint32_t>(indices.size());

	// Create a key made of the attributes, to see if we cache the primitive then we just reused it 
	std::stringstream o;
	for(auto& a: tmesh.attributes)
	{
		o << a.first << a.second;
	}
	std::string key            = o.str();
	bool        primMeshCached = false;

	// Found a cache - will not need to append vertex
	auto it = cachePrimMesh.find(key);
	if(it != cachePrimMesh.end())
	{
		primMeshCached          = true;
		GltfPrimMesh cacheMesh  = it->second;
		resultMesh.vertexCount  = cacheMesh.vertexCount;
		resultMesh.vertexOffset = cacheMesh.vertexOffset;
	}

	// INDICES
	if(tmesh.indices > -1)
	{
		const tinygltf::Accessor& indexAccessor = tmodel.accessors[tmesh.indices];
		resultMesh.indexCount                   = static_cast<uint32_t>(indexAccessor.count);

		switch(indexAccessor.componentType)
		{
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:{
				primitiveIndices32u.resize(indexAccessor.count);
				copyAccessorData(primitiveIndices32u, 0, tmodel, indexAccessor, 0, indexAccessor.count);
				indices.insert(indices.end(), primitiveIndices32u.begin(), primitiveIndices32u.end());
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:{
				primitiveIndices16u.resize(indexAccessor.count);
				copyAccessorData(primitiveIndices16u, 0, tmodel, indexAccessor, 0, indexAccessor.count);
				indices.insert(indices.end(), primitiveIndices16u.begin(), primitiveIndices16u.end());
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:{
				primitiveIndices8u.resize(indexAccessor.count);
				copyAccessorData(primitiveIndices8u, 0, tmodel, indexAccessor, 0, indexAccessor.count);
				indices.insert(indices.end(), primitiveIndices8u.begin(), primitiveIndices8u.end());
				break;
			}
			default:
				LogE("Index component type "+std::to_string(indexAccessor.componentType)+" not support!");
				return;
		}
	}
	else 
	{
		// Primitive without indices, creating them
		const auto& accessor = tmodel.accessors[tmesh.attributes.find("POSITION")->second];
		for(auto i = 0; i < accessor.count; i++)
			indices.push_back(i);
		resultMesh.indexCount = static_cast<uint32_t>(accessor.count);
	}

	if(primMeshCached == false) // Need to add this primitive
	{
		// POSITION
		{
			const bool hadPosition = getAttribute<glm::vec3>(tmodel, tmesh, positions, "POSITION");
			if(!hadPosition)
			{
				LogE("This gltf have no POSITION attribute.\n");
				return;
			}
			// keep the size of this primitive
			const auto& accessor   = tmodel.accessors[tmesh.attributes.find("POSITION")->second];
			resultMesh.vertexCount = static_cast<uint32_t>(accessor.count);
			if(!accessor.minValues.empty())
			{
				resultMesh.posMin = glm::vec3(accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
			}
			else
			{
				resultMesh.posMin = glm::vec3(std::numeric_limits<float>::max());
				for(const auto& p : positions)
				{
					for(int i = 0; i < 3; i++)
					{
						if(p[i] < resultMesh.posMin[i])
							resultMesh.posMin[i] = p[i];
					}
				}
			}
			if(!accessor.maxValues.empty())
			{
				resultMesh.posMax = glm::vec3(accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
			}
			else 
			{
				resultMesh.posMax = glm::vec3(-std::numeric_limits<float>::max());
				for(const auto& p : positions)
				{
					for(int i = 0; i < 3; i++)
					{
						if(p[i] > resultMesh.posMax[i])
							resultMesh.posMax[i] = p[i];
					}
				}
			}

		}


		// NORMAL
		if(hasFlag(requestedAttributes, GltfAttributes::Normal))
		{
			bool normalCreated = getAttribute<glm::vec3>(tmodel, tmesh, normals, "NORMAL");

			if(!normalCreated && hasFlag(forceRequested, GltfAttributes::Normal))
				createNormals(resultMesh);
		}

		// TEXCOORD_0
		if(hasFlag(requestedAttributes, GltfAttributes::Texcoord_0))
		{
			bool texcoordCreated = getAttribute<glm::vec2>(tmodel, tmesh, texcoords0, "TEXCOORD_0");
			if(!texcoordCreated)
				texcoordCreated = getAttribute<glm::vec2>(tmodel, tmesh, texcoords0, "TEXCOORD");
			if(!texcoordCreated && hasFlag(forceRequested, GltfAttributes::Texcoord_0))
				createTexcoords(resultMesh);
		}


		// TANGENT
		if(hasFlag(requestedAttributes, GltfAttributes::Tangent))
		{
			std::vector<glm::vec4> gltfTangents;
			bool tangentCreated = getAttribute<glm::vec4>(tmodel, tmesh, gltfTangents, "TANGENT");

			if(!tangentCreated && hasFlag(forceRequested, GltfAttributes::Tangent))
			{
				//Log("No tangent found, creating them");
				createTangents(resultMesh);
			}
			else
			{
				//Log("Tangent found, using them");
				std::vector<glm::vec3> tangent;
				std::vector<glm::vec3> bitangent;

				for(uint32_t a = 0; a < resultMesh.vertexCount; a++)
				{
					const auto& gt = gltfTangents[a];
					const auto& n = normals[resultMesh.vertexOffset + a];

					glm::vec3 t = gt;
					glm::vec3 b = glm::cross(n, t);

					if(gt.w == -1.0)
					{
						t = -t;
						b = -b;
						
					}

					
					tangents.push_back(t);
					bitangents.push_back(b);



				}

			}
				
		}


		// COLOR_0
		if(hasFlag(requestedAttributes, GltfAttributes::Color_0))
		{
			bool colorCreated = getAttribute<glm::vec4>(tmodel, tmesh, colors0, "COLOR_0");
			if(!colorCreated && hasFlag(forceRequested, GltfAttributes::Color_0))
				createColors(resultMesh);
		}
	}
	
	cachePrimMesh[key] = resultMesh;

	primMeshes.emplace_back(resultMesh);

}


// Get the dimension of the scene
void GltfLoader::computeSceneDimensions()
{
	BBox scenceBBox;

	for(const auto& node : nodes)
	{
		const auto& mesh = primMeshes[node.primMesh];

		BBox bbox(mesh.posMin, mesh.posMax);
		bbox = bbox.transform(node.worldMatrix);
		scenceBBox.insert(bbox);
	}

	if(scenceBBox.isEmpty() || !scenceBBox.isVolume())
	{
		LogE("gltf: Scene bounding box invalid , Setting to: [-1,-1,-1], [1,1,1]");
		scenceBBox.insert({-1.0f, -1.0f, -1.0f});
		scenceBBox.insert({1.0f, 1.0f, 1.0f});
	}

	dimensions.min    = scenceBBox.getMin();
	dimensions.max    = scenceBBox.getMax();
	dimensions.size   = scenceBBox.extents();
	dimensions.center = scenceBBox.center();
	dimensions.radius = scenceBBox.radius();


}

void GltfLoader::computeCamera()
{
	for(auto& camera : cameras)
	{
		if(camera.eye == camera.center)
		{
			camera.worldMatrix = glm::translate(glm::mat4{1},camera.eye);
			float distance = glm::length(dimensions.center - camera.eye);
			glm::mat3  rotMat{1};
			camera.center = {0, 0, -distance};
			camera.center = camera.eye + (rotMat * camera.center);
			camera.up     = {0, 1, 0};
		}
	}
}

void GltfLoader::checkRequiredExtensions(const tinygltf::Model& tmodel)
{
	std::set<std::string> supportedExtensions{
		KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME
	};

	for(auto& e : tmodel.extensionsRequired)
	{
		if(supportedExtensions.find(e) == supportedExtensions.end())
		{
			LogW("The extension " + e + " is REQUIRED and not supported");
		}

	}
}

void GltfLoader::findUsedMeshes(const tinygltf::Model& tmodel, std::set<uint32_t>& usedMeshes, int nodeIdx)
{
	const auto& node = tmodel.nodes[nodeIdx];
	if(node.mesh >= 0)
		usedMeshes.insert(node.mesh);
	for(const auto& c : node.children)
		findUsedMeshes(tmodel, usedMeshes, c);
}

void GltfLoader::createNormals(GltfPrimMesh& resultMesh)
{
	// Compute the normals
	std::vector<glm::vec3> geonormal(resultMesh.vertexCount);
	for(size_t i = 0; i < resultMesh.indexCount; i += 3)
	{
		uint32_t      ind0 = indices[resultMesh.firstIndex + i + 0];
		uint32_t      ind1 = indices[resultMesh.firstIndex + i + 1];
		uint32_t      ind2 = indices[resultMesh.firstIndex + i + 2];
		const auto&   pos0 = positions[ind0 + resultMesh.vertexOffset];
		const auto&   pos1 = positions[ind1 + resultMesh.vertexOffset];
		const auto&   pos2 = positions[ind2 + resultMesh.vertexOffset];
		const auto    v1   = glm::normalize(pos1 - pos0);
		const auto    v2   = glm::normalize(pos2 - pos0);
		const auto    n    = glm::cross(v1, v2);
		geonormal[ind0]   += n;
		geonormal[ind1]   += n;
		geonormal[ind2]   += n;
	}
	for(auto& n: geonormal)
		n = glm::normalize(n);

	normals.insert(normals.end(), geonormal.begin(), geonormal.end());
}

void GltfLoader::createTexcoords(GltfPrimMesh& resultMesh)
{
	// Cube map projection
	for(uint32_t i = 0; i < resultMesh.vertexCount; i++)
	{
		const auto& pos  = positions[resultMesh.vertexOffset + i];
		float       absX = fabs(pos.x);
		float       absY = fabs(pos.y);
		float       absZ = fabs(pos.z);

		int isXPositive = pos.x > 0 ? 1 : 0;
		int isYPositive = pos.y > 0 ? 1 : 0;
		int isZPositive = pos.z > 0 ? 1 : 0;

		float maxAxis{}, uc{}, vc{};   // Zero-initialize in case pos = {NaN, NaN, NaN}

		// POSITIVE X
		if(isXPositive && absX >= absY && absX >= absZ)
		{
			// u (0 to 1) goes from +z to -z
			// v (0 to 1) gose from -y to +y
			maxAxis = absX;
			uc      = -pos.z;
			vc      = pos.y;
		}
		// NEGATIVE X
		if(!isXPositive && absX >= absY && absX >= absZ)
		{
			// u (0 to 1) goes from -z to +z
			// v (0 to 1) gose from -y to +y
			maxAxis = absX;
			uc      = pos.z;
			vc      = pos.y;
		}
		// POSITIVE Y
		if(isYPositive && absY >= absX && absY >= absZ)
		{
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) gose from +z to -z
			maxAxis = absY;
			uc      = pos.x;
			vc      = -pos.z;
		}
		// NEGATIVE Y
		if(!isYPositive && absY >= absX && absY >= absZ)
		{
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) gose from -z to +z
			maxAxis = absY;
			uc      = pos.x;
			vc      = pos.z;
		}
		// POSITIVE Z
		if(isZPositive && absZ >= absX && absZ >= absY)
		{
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) gose from -y to +y
			maxAxis = absZ;
			uc      = pos.x;
			vc      = pos.y;
		}
		// NEGATIVE Z
		if(!isZPositive && absZ >= absX && absZ >= absY)
		{
			// u (0 to 1) goes from +x to -x
			// v (0 to 1) gose from -y to +y
			maxAxis = absZ;
			uc      = -pos.x;
			vc      = pos.y;
		}

		// Convert range from -1 to 1 to 0 to 1
		float u = 0.5f * (uc / maxAxis + 1.0f);
		float v = 0.5f * (vc / maxAxis + 1.0f);

		texcoords0.emplace_back(u,v);
	}
}

void GltfLoader::createTangents(GltfPrimMesh& resultMesh)
{
	std::vector<glm::vec3> tangent(resultMesh.vertexCount);
	std::vector<glm::vec3> bitangent(resultMesh.vertexCount);

	// Reference  http://foundationsofgameenginedev.com/FGED2-sample.pdf
	for(size_t i = 0; i < resultMesh.indexCount; i += 3)
	{
		// local index
		uint32_t i0 = indices[resultMesh.firstIndex + i + 0];
		uint32_t i1 = indices[resultMesh.firstIndex + i + 1];
		uint32_t i2 = indices[resultMesh.firstIndex + i + 2];
		assert(i0 < resultMesh.vertexCount);
		assert(i1 < resultMesh.vertexCount);
		assert(i2 < resultMesh.vertexCount);

		// global index
		uint32_t gi0 = i0 + resultMesh.vertexOffset;
		uint32_t gi1 = i1 + resultMesh.vertexOffset;
		uint32_t gi2 = i2 + resultMesh.vertexOffset;

		const auto& p0 = positions[gi0];
		const auto& p1 = positions[gi1];
		const auto& p2 = positions[gi2];

		const auto& uv0 = texcoords0[gi0];
		const auto& uv1 = texcoords0[gi1];
		const auto& uv2 = texcoords0[gi2];

		glm::vec3 e1 = p1 - p0;
		glm::vec3 e2 = p2 - p0;

		glm::vec2 duvE1 = uv1 - uv0;
		glm::vec2 duvE2 = uv2 - uv0;

		float r = 1.0f;
		float a = duvE1.x * duvE2.y - duvE2.x * duvE1.y;
		if(fabs(a) > 0) // catch degenerated UV
		{
			//Log("Degenerated UV");
			r = 1.0f / a;
		}

		glm::vec3 t = (e1 * duvE2.y - e2 * duvE1.y) * r;
		glm::vec3 b = (e2 * duvE1.x - e1 * duvE2.x) * r;

		tangent[i0] += t;
		tangent[i1] += t;
		tangent[i2] += t;

		bitangent[i0] += b;
		bitangent[i1] += b;
		bitangent[i2] += b;
	}

	for(uint32_t a = 0; a < resultMesh.vertexCount; a++)
	{
		const auto& t = tangent[a];
		const auto& b = bitangent[a];
		const auto& n = normals[resultMesh.vertexOffset + a];

		// Gram-Schmidt orthogonalize
		glm::vec3 otangent = glm::normalize(t - (glm::dot(n, t) * n));

		// In case the tangent is invalid
		if(otangent == glm::vec3(0, 0, 0))
		{
			//Log("Invalid tangent, creating a new one");
			if(fabsf(n.x) > fabsf(n.y))
				otangent = glm::vec3(n.z, 0, -n.x) / sqrtf(n.x * n.x + n.z * n.z);
			else 
				otangent = glm::vec3(0, -n.z, n.y) / sqrtf(n.y * n.y + n.z * n.z);
		}

		// Calculate handedness
		float handedness = (glm::dot(glm::cross(n, t), b) < 0.0f) ? 1.0f : -1.0f;

		
		

		glm::vec3 obitangent = glm::normalize(glm::cross(n, otangent) * handedness);

		//if(handedness == -1.0f)
		//{
		//	otangent   = -otangent;
		//	obitangent = -obitangent;
		//}

		tangents.emplace_back(otangent.x, otangent.y, otangent.z);
		bitangents.emplace_back(obitangent.x, obitangent.y, obitangent.z);

	}
}

void GltfLoader::createColors(GltfPrimMesh& resultMesh)
{
	// Set them all to one
	colors0.insert(colors0.end(), resultMesh.vertexCount, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}


glm::mat4 getLocalMatrix(const tinygltf::Node& tnode)
{
	glm::mat4 mtranslation{1};
	glm::mat4 mscale{1};
	glm::mat4 mrot{1};
	glm::mat4 matrix{1};
	
	

	glm::vec4 vrotation[4];


	if(!tnode.translation.empty())
		mtranslation = glm::translate(mtranslation,{tnode.translation[0], tnode.translation[1], tnode.translation[2]});
	if(!tnode.scale.empty())
		mscale = glm::scale(mscale, {tnode.scale[0], tnode.scale[1], tnode.scale[2]});
	if(!tnode.rotation.empty())
	{
		glm::quat rotation;

		std::transform(tnode.rotation.begin(), tnode.rotation.end(), glm::value_ptr(rotation), TypeCast<double, float>{});
		//glm::quat mrotation = glm::quat(tnode.rotation[3], tnode.rotation[0], tnode.rotation[1], tnode.rotation[2]);

		mrot = glm::mat4_cast(rotation);

	}
	if(!tnode.matrix.empty())
	{
		// Maybe wrong direction!!!
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				matrix[i][j] = static_cast<float>(tnode.matrix[i * 4 + j]);
			}
		}
	}

	//return matrix * mscale * mrot * mtranslation;
	return mtranslation * mrot * mscale * matrix;
}
