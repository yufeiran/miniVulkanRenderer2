#include"material.h"

namespace mini {

	Material toMaterial(const tinyobj::material_t& m) {
		Material mat;
		mat.name = m.name;
		for (int i = 0; i < 3; i++)
		{
			mat.ambient[i] = m.ambient[i];
			mat.diffuse[i] = m.diffuse[i];
			mat.transmittance[i] = m.transmittance[i];
			mat.emission[i] = m.emission[i];

		}
		mat.shininess = m.shininess;
		mat.ior = m.ior;
		mat.dissolve = m.dissolve;
		mat.ambient_texname = m.ambient_texname;
		mat.diffuse_texname = m.diffuse_texname;
		mat.emission_texname = m.emissive_texname;
		mat.specular_texname = m.specular_texname;
		mat.specular_highlight_texname = m.specular_highlight_texname;

		return mat;
	}
}