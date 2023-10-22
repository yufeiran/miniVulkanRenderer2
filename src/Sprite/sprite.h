#pragma once
#include"Common/common.h"

namespace mini
{
class Model;

enum SPRITE_STATE {
	SHOW_SPRITE_STATE,
	HIDE_SPRITE_STATE,
	REMOVE_SPRITE_STATE
};

class Sprite
{
public:
	Sprite(uint32_t id, Model& model, float x=0, float y=0, float z=0, float scale=1, float angleX=0,float angleY=0,float angleZ=0, std::string name=std::string());
	Model& getModel() const;
	void calModelMat();
	glm::mat4 getModelMat()const;

private:
	glm::mat4 modelMat;
	std::string name;
	uint32_t id;
	Model& model;
	SPRITE_STATE state;
	float x,y,z; // x,y,z in world mat
	float scale;
	float angleX, angleY, angleZ;
};

}