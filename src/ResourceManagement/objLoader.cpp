#include "objLoader.h"

using namespace mini;

void ObjLoader::loadModel(const std::string& filename)
{
	tinyobj::ObjReader reader;
	reader.ParseFromFile(filename);

	if(!reader.Valid())
	{
		Log("Cannot load file:"+filename,LOG_TYPE::ERROR_TYPE);
	}

	for(const auto& material : reader.GetMaterials())
	{
		MaterialObj m;
		
		m.ambient          = glm::vec3(material.ambient[0],material.ambient[1],material.ambient[2]);
		m.diffuse          = glm::vec3(material.diffuse[0],material.diffuse[1],material.diffuse[2]);
		m.specular         = glm::vec3(material.specular[0],material.specular[1],material.specular[2]);
		m.transmittance    = glm::vec3(material.transmittance[0],material.transmittance[1],material.transmittance[2]);
		m.dissolve         = material.dissolve;
		m.ior              = material.ior;
		m.shininess        = material.shininess;
		m.illum            = material.illum;
		if(!material.diffuse_texname.empty())
		{
			textures.push_back(material.diffuse_texname);
			m.textureID = static_cast<int>(textures.size()) - 1;
		}

		materials.emplace_back(m);
	}

	// if the model dont have material , add a default
	if(materials.empty())
	{
		materials.emplace_back(MaterialObj());
	}

	const tinyobj::attrib_t& attrib = reader.GetAttrib();

	for(const auto& shape:reader.GetShapes())
	{
		vertices.reserve(shape.mesh.indices.size() + vertices.size());
		indices.reserve(shape.mesh.indices.size() + indices.size());
		matIndx.insert(matIndx.end(),shape.mesh.material_ids.begin(),shape.mesh.material_ids.end());

		for(const auto& index : shape.mesh.indices)
		{
			VertexObj     vertex = {};
			const float*  vp     = &attrib.vertices[3 * index.vertex_index];
			vertex.pos           = {*(vp + 0),*(vp + 1),*(vp + 2)};

			if(!attrib.normals.empty() && index.normal_index >= 0)
			{
				const float* np = &attrib.normals[3 * index.normal_index];
				vertex.normal   = {*(np + 0), *(np + 1), *(np + 2)};
			}

			if(!attrib.texcoords.empty() && index.texcoord_index >= 0)
			{
				const float* tp = &attrib.texcoords[2 * index.texcoord_index + 0];
				vertex.texCoord = {*tp, 1.0f - *(tp + 1)};
			}

			if(!attrib.colors.empty())
			{
				const float* vc = &attrib.colors[3 * index.vertex_index];
				vertex.color    = {*(vc + 0), *(vc + 1), *(vc + 2), 1.0f};
			}

			vertices.push_back(vertex);
			indices.push_back(static_cast<int>(indices.size()));
		}
	}

	// Fix material indices
	for(auto& mi : matIndx)
	{
		if(mi < 0 || mi > materials.size())
		{
			mi = 0;
		}
	}

	// Compute normal when no normal were provided 
	if(attrib.normals.empty())
	{
		for(size_t i = 0; i < indices.size(); i += 3)
		{
			VertexObj& v0 = vertices[indices[i + 0]];
			VertexObj& v1 = vertices[indices[i + 1]];
			VertexObj& v2 = vertices[indices[i + 2]];

			glm::vec3 n = glm::normalize(glm::cross((v1.pos - v0.pos),(v2.pos - v0.pos)));
			v0.normal   = n;
			v1.normal   = n;
			v2.normal   = n;
		}
	}

}
