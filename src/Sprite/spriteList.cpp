#include"spriteList.h"

namespace mini
{


void SpriteList::addSprite(Model& model, float x, float y, float z, float scale, float angleX, float angleY, float angleZ)
{
	Sprite sprite(nowIndex++, model, x, y, z, scale, angleX, angleY, angleZ);
	sprites.push_back(sprite);
}

}