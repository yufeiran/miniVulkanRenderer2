#pragma once 
#include"Common/common.h"
#include"Common/vertex.h"
#include"Common/material.h"
#include"shape.h"

namespace mini
{




class Buffer;
class Device;
class Image;
class ImageView;


//要改成多shape
class Model
{
public:
	Model(Device&device,const std::string& filePath, const std::string& name,uint32_t id, bool flipTexture=false);
	~Model();
	
	std::string getName() const;

	uint32_t getID() const;

	const std::map<std::string, std::unique_ptr<Shape>>& getShapeMap()const;

	void loadImage(const std::string& name, bool flipTexture=false);

	ImageView& getImageViewByName(const std::string& name);

	std::map<std::string, std::unique_ptr<Shape>> shapeMap;

private:
	std::string filePath;
	std::string baseDirPath;
	std::string name;
	uint32_t id;
	Device& device;




	std::map<std::string, Material> materials;






	std::map<std::string, std::unique_ptr<Image>>imageMap;
	std::map<std::string, std::unique_ptr<ImageView>>imageViewMap;


};

}