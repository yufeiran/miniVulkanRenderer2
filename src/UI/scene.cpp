#include<UI/scene.h>

namespace mini
{

	void Scene::loadScene(int testCase, ResourceManager& rm, Camera& camera)
	{
		switch (testCase)
		{
		case 0:
			loadShowCase(rm, camera);
			break;
		case 1:
			loadTestGltf(rm, camera);
			break;
		case 2:
			loadSponza(rm, camera);
			break;
		case 3:
			loadBugBox(rm, camera);
			break;
		case 4:
			loadFeatures(rm, camera);
			break;
		}
	}

	void Scene::loadBugBox(ResourceManager& rm, Camera& camera)
	{
		glm::mat4 objMat = glm::mat4(1.0f);
		objMat = glm::mat4(1.0f);
		//resourceManager->loadScene("E://yufeiran/model/AMD/Deferred/Deferred.gltf",objMat);

		rm.loadScene("E://yufeiran/model/bugbox/bugbox.gltf", objMat);

		rm.loadScene(getAssetPath("lightScene.gltf"));
	}
	void Scene::loadTestGltf(ResourceManager& rm, Camera& camera)
	{
		glm::mat4 objMat = glm::mat4(1.0f);
		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0, 5, -3 });
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/BoxVertexColors/glTF/BoxVertexColors.gltf", objMat);

		//
		//resourceManager->loadScene("E://yufeiran/model/AMD/Robot/Robot.gltf");


		objMat = glm::translate(objMat, { 2,0,0 });
		objMat = glm::scale(objMat, { 5,5,5 });
		//resourceManager->loadObjModel("bunny", getAssetPath("bunny/bunny.obj",objMat);

		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 4,2.5,0 });
		objMat = glm::scale(objMat, { 2,2,2 });
		//resourceManager->loadObjModel("smpl", getAssetPath("smpl/smpl.obj",objMat);

		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,2,0 });
		objMat = glm::scale(objMat, { 1,1,1 });
		//resourceManager->loadObjModel("backpack", "../../assets/backpack/backpack.obj",objMat, true);

		//resourceManager->loadScene(getAssetPath("glTFBox/Box.gltf",objMat);

		//resourceManager->loadScene(getAssetPath("glTFBox/Box.gltf");
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf");

		//objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{-12,5,0});
		//resourceManager->loadScene(getAssetPath("cornellBox/cornellBox.gltf",objMat);
		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,0,0 });
		objMat = glm::scale(objMat, { 3,1,3 });
		rm.loadScene("../../assets/plane/plane1.gltf", objMat);


		//resourceManager->loadScene(getAssetPath("lightScene.gltf");


		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,3,0 });
		objMat = glm::scale(objMat, { 3,3,3 });
		rm.loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/BoxTextured/glTF/BoxTextured.gltf", objMat);


		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,2,0 });
		objMat = glm::scale(objMat, { 1,1,1 });
		rm.loadObjModel("backpack", "../../assets/backpack/backpack.obj", objMat, true);



		//objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{0, 8, -3});
		////resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/Duck/glTF/Duck.gltf", objMat);

		//



		//objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{5, 2, 1});
		//objMat = glm::scale(objMat, {0.005, 0.005, 0.005});


		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf", objMat);

		//objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{-15, 0, 0});
		//objMat = glm::scale(objMat, {0.1, 0.1, 0.1});

		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/GearboxAssy/glTF/GearboxAssy.gltf", objMat);


		//objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{5, 5, 1});
		//objMat = glm::scale(objMat, {0.01, 0.01, 0.01});
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/Buggy/glTF/Buggy.gltf",objMat);




		camera.setPos(glm::vec3(-0.0, 0, 15.0));
		camera.setViewDir(-90, 0);
		//pcRaster.lightPosition = glm::vec3(0, 4.5f, 0.f);



		//resourceManagement->loadModel("Medieval_building", getAssetPath("nv_raytracing_tutorial_scene/Medieval_building.obj",true);

		//resourceManagement->loadModel("plane", getAssetPath("nv_raytracing_tutorial_scene/plane.obj",true);
	}
	void Scene::loadShowCase(ResourceManager& rm, Camera& camera)
	{
		glm::mat4 objMat = glm::mat4(1.0f);
		objMat = glm::mat4(1.0f);
		//objMat = glm::scale(objMat, { 0.01, 0.01, 0.01 });

		objMat = glm::translate(objMat, { 2,0,2 });
		objMat = glm::scale(objMat, { 50,50,50 });
		rm.loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/BoomBox/glTF/BoomBox.gltf", objMat);
		////resourceManager->loadScene("E://yufeiran/model/AMD/GI/GI.gltf",objMat);
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/GearboxAssy/glTF/GearboxAssy.gltf",objMat);
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/AntiqueCamera/glTF/AntiqueCamera.gltf",objMat);

		//objMat = glm::mat4(1.0f);
		//objMat = glm::scale(objMat, { 2, 2, 2 });

		//objMat = glm::translate(objMat, { 0,1,1 });
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/BarramundiFish/glTF/BarramundiFish.gltf",objMat);

		//objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{0,0,3});
		//objMat = glm::scale(objMat, {20, 20, 20});


		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/BoomBox/glTF/BoomBox.gltf",objMat);

		////resourceManager->loadScene(getAssetPath("lightScene.gltf");



		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0, 0, 0 });
		////resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

		rm.loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf", objMat);





		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,-1,0 });
		objMat = glm::scale(objMat, { 5,1,5 });
		//resourceManager->loadScene(getAssetPath("texturePlane/texturePlane.gltf", objMat);

		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,-1,0 });
		objMat = glm::scale(objMat, { 1,1, 1 });
		rm.loadScene(getAssetPath("plane/plane.gltf"), objMat);

		//resourceManager->loadScene("E://yufeiran/model/rocky_ground_with_moss/scene.gltf", objMat);

		objMat = glm::mat4(1.0f);
		//resourceManager->loadScene("E://yufeiran/model/heitaSpace/heitaSpace.gltf", objMat);
	}
	void Scene::loadSponza(ResourceManager& rm, Camera& camera)
	{
		glm::mat4 objMat = glm::mat4(1.0f);
		objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat,{-10,-1,0});
		//rm->loadScene("E://yufeiran/model/AMD/Deferred/Deferred.gltf",objMat);

		//rm->loadScene("E://yufeiran/model/AMD/GI/GI.gltf", objMat);

		rm.loadScene("E://yufeiran/model/AMD/PBR/PBR.gltf", objMat);
		//rm->loadScene("E://yufeiran/model/AMD/Caustics/Caustics.gltf", objMat);
		//rm->loadScene("E://yufeiran/model/Inazuma/gltf/Inazuma.gltf", objMat);
		//rm->loadScene("E://yufeiran/model/mingshen/mingshen/gltf/mingshen.gltf", objMat);

		//rm->loadScene("E://yufeiran/model/Copper/Copper.gltf", objMat);
		//rm->loadScene("E://yufeiran/model/glass/glass.gltf", objMat);

		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,-1.5,0 });
		objMat = glm::scale(objMat, { 1,1, 1 });
		//rm->loadScene(getAssetPath("plane/plane.gltf"), objMat);

		//rm->loadScene("E://yufeiran/model/AMD/Shadow/Shadow.gltf", objMat);

		//rm->loadScene("E://yufeiran/model/debug/flower.gltf", objMat);

		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf",objMat);

		//resourceManager->loadScene(getAssetPath("lightScene.gltf");
	}
	void Scene::loadFeatures(ResourceManager& rm, Camera& camera)
	{
		glm::mat4 objMat = glm::mat4(1.0f);
		objMat = glm::mat4(1.0f);
		//objMat = glm::translate(objMat, glm::vec3(0.0,10.0,0.0));
		//objMat = glm::rotate(objMat,glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
		//objMat = glm::scale(objMat,{0.01,0.01,0.01});
		//resourceManager->loadScene("E://yufeiran/model/AMD/GI/GI.gltf",objMat);
		//resourceManager->loadScene("E://yufeiran/model/bug/greenballs.gltf",objMat);
		//resourceManager->loadScene("E://yufeiran/model/bug/allwithoutcloth.gltf",objMat);

		//resourceManager->loadScene("E://yufeiran/model/bug/clothbackdrop.gltf",objMat);
		//resourceManager->loadScene("E://yufeiran/model/bug/clothpart1.gltf",objMat);

		// MetalRoughSpheres Test
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf",objMat);

		// TransmissionTest
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/TransmissionTest/glTF/TransmissionTest.gltf",objMat);
		//BoomBoxWithAxes
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf",objMat);

			// Alpha Test
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/AlphaBlendModeTest/glTF/AlphaBlendModeTest.gltf",objMat);

		// NegativeScaleTest
		// resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/NegativeScaleTest/glTF/NegativeScaleTest.gltf",objMat);

		//NormalTangentMirrorTest
		//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/NormalTangentMirrorTest/glTF/NormalTangentMirrorTest.gltf",objMat);

		//resourceManager->loadScene("E://yufeiran/model/bug/bugTangentRect.gltf",objMat);


		//NormalTangentTest 
		rm.loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/NormalTangentTest/glTF/NormalTangentTest.gltf", objMat);


		// TransmissionTest
	//resourceManager->loadScene("E://yufeiran/model/glTF-Sample-Models/2.0/TransmissionTest/glTF/TransmissionTest.gltf",objMat);

		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, glm::vec3(0.0, 3.0, 0.0));
		objMat = glm::rotate(objMat, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));






		//resourceManager->loadScene(getAssetPath("lightScene.gltf");

		objMat = glm::mat4(1.0f);
		objMat = glm::translate(objMat, { 0,-1,0 });
		objMat = glm::scale(objMat, { 3,1,3 });
		//resourceManager->loadScene( getAssetPath("plane/plane1.gltf",objMat);
	}

	void Scene::loadCubeMap(ResourceManager& rm)
	{

		std::vector<std::string> defaultCubeMapNames = {
		getAssetPath("skybox/default/right.jpg"),
		getAssetPath("skybox/default/left.jpg"),
		getAssetPath("skybox/default/top.jpg"),
		getAssetPath("skybox/default/bottom.jpg"),
		getAssetPath("skybox/default/front.jpg"),
		getAssetPath("skybox/default/back.jpg"),
		};

		std::vector<std::string> yokohamaCubeMapNames = {
			getAssetPath("skybox/Yokohama3/posx.jpg"),
			getAssetPath("skybox/Yokohama3/negx.jpg"),
			getAssetPath("skybox/Yokohama3/posy.jpg"),
			getAssetPath("skybox/Yokohama3/negy.jpg"),
			getAssetPath("skybox/Yokohama3/posz.jpg"),
			getAssetPath("skybox/Yokohama3/negz.jpg")
		};


		std::vector<std::string> HornstullsStrandCubeMapNames = {
		getAssetPath("skybox/HornstullsStrand/posx.jpg"),
		getAssetPath("skybox/HornstullsStrand/negx.jpg"),
		getAssetPath("skybox/HornstullsStrand/posy.jpg"),
		getAssetPath("skybox/HornstullsStrand/negy.jpg"),
		getAssetPath("skybox/HornstullsStrand/posz.jpg"),
		getAssetPath("skybox/HornstullsStrand/negz.jpg")
		};

		std::string dikhololo_night_4k_Names = getAssetPath("HDRI/dikhololo_night_4k.hdr");

		std::string dikhololo_night_8k_Names = getAssetPath("HDRI/dikhololo_night_8k.hdr");

		std::string qwantani_moon_noon_puresky_4k_Names = getAssetPath("HDRI/qwantani_moon_noon_puresky_4k.hdr");


		//rm.loadCubemap(HornstullsStrandCubeMapNames);
		rm.loadHDR(qwantani_moon_noon_puresky_4k_Names);
	}
}