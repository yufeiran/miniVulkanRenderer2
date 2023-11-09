#pragma once
#include"Common/common.h"

namespace mini
{

enum CAMERA_MOVE_DIR
{
	UP_DIR=0, //前进
	DOWN_DIR, //后退
	LEFT_DIR, 
	RIGHT_DIR,
	FRONT_DIR, //上
	END_DIR   //下
};
class Camera
{
public:



	Camera(glm::vec3 cameraPos=glm::vec3(2,2,2), glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f),
		double yaw = -135, double pitch = -45, double roll = 0,float fov = 90, double step = 3, double changeDirSensitivity=0.05);

	void calViewMat();
	void updateCameraVec();

	void move(CAMERA_MOVE_DIR dir,float deltaTime);

	void changeDir(double xoffset, double yoffset);

	glm::mat4 getViewMat() const;

	float getFov() { return fov; }

private:
	glm::vec3 cameraPos, cameraFront, cameraUp;
	glm::vec3 cameraRight;
	glm::vec3 worldUp;
	glm::vec3 direction;
	glm::mat4 viewMat;
	double yaw, pitch, roll;
	float step; 
	float changeDirSensitivity;
	float fov;

};

}