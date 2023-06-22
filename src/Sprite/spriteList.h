#pragma once
#include"Common/common.h"
#include"sprite.h"

namespace mini
{
	class Sprite;
	class Model;

	class SpriteList
	{
	public:
		void addSprite(Model& model, float x = 0, float y = 0, float z = 0, float scale = 1, float angleX = 0, float angleY = 0, float angleZ = 0);
		

		
		std::vector<Sprite> sprites;

	private:

		uint32_t nowIndex;
	};


}