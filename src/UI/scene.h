#pragma once

#include <Common/common.h>
#include <ResourceManagement/ResourceManager.h>
#include <Common/camera.h>

namespace mini
{
	class Scene
	{
	public:
		void loadScene(int testCase, ResourceManager& rm, Camera& camera);

		void loadBugBox(ResourceManager& rm, Camera& camera);
		void loadTestGltf(ResourceManager& rm, Camera& camera);
		void loadShowCase(ResourceManager& rm, Camera& camera);
		void loadSponza(ResourceManager& rm, Camera& camera);
		void loadFeatures(ResourceManager& rm, Camera& camera);

		void loadCubeMap(ResourceManager& rm);
	private:

	};
}