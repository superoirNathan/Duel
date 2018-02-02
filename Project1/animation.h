#pragma once
#include "Mesh.h"
#include <memory>
#include <vector>

class Animation {
public:
	void initialize();
	
	float t;
	int currentFrame = 0;


	std::vector<std::shared_ptr<Mesh>> keyFrames;
private:


};