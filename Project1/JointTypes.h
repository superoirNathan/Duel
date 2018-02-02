// Various types of classes and structures needed for representing joints for kinematic hierarchies
#pragma once

#include <GLM/glm.hpp>

enum RotationOrder
{
	// The two we typically care about
	XYZ = 0,
	ZYX,

	// But technically, rotation can happen in any order.
	XZY,
	YXZ,
	YZX

	// ... 
};

// This is import to ensure the animation is scaled correctly
// We will use Centimeters as our working units. It doesn't matter what
// you use, as long as your consistent between Blender, Maya and Motion builder
enum CalibrationUnits
{
	Millimeters,
	Centimeters
};

struct JointDescriptor
{
	JointDescriptor()
	{
		boneLength = 0;
		numFrames = 0;
	}

	std::string jointName;			// Name of joint
	std::string jointParent;		// Name of joint's parent
	glm::vec3	jointBasePosition;	// Position of joint at frame 0
	glm::quat jointBaseRotation;	// Rotation of joint at frame 0
	float	boneLength;

	std::vector<glm::vec3>	jointPositions;		// Joint point for each frame (should all be zero except for root)
	std::vector<glm::quat> jointRotations;	// Joint rotations for each frame 
	std::vector<float> jointScales;

	unsigned int numFrames;

	// Skin weights
	// Stored as a "vector of pairs containing a vertex index (int) and influence (float)"
	std::vector<std::pair<int, float>> skinWeights;
};
