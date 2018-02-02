//Nathan Alphonse 100523109

#include "HTRLoader.h"
#include <iostream>

#define MAX_LINE_LENGTH 128

HTRLoader::HTRLoader()
{
	rootGameObject = nullptr;
}

bool HTRLoader::loadHTR(std::string fileName)
{
	FILE* file;
	file = fopen(fileName.c_str(), "r");

	char* loc;
	char buffer[MAX_LINE_LENGTH];
	int bufferSize = 0;

	if (file)
	{
		// Find line with header
		// fgets() will read to new line OR MAX_LINE_LENGTH, which ever happens first
		// Returns pointer to buffer OR null pointer for end of file
		while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL)
		{
			// Get location of "Header" in file
			loc = strstr(buffer, "Header");

			// Process headers
			if (loc)
			{
				if (!processHeaderSection(file, loc))
					break;
				
				if (!processSegementNameSection(file, loc))
					break;

				if (!processBasePositionSection(file, loc)) // TODO: IMPLEMENT THIS FUNCTION
					break;

				if (!processAnimationSection(file, loc)) // TODO: IMPLEMENT THIS FUNCTION
					break;
			}
		}

		fclose(file);	// Remember to close file

		return true;
	}
	else
	{
		std::cout << "LOAD HTR ERROR: " << fileName << " not found or cannot be opened." << std::endl;
		assert(0);
		return false;
	}
}

JointDescriptor* HTRLoader::getJointDescriptorByName(std::string jointName)
{
	int numJoints = jointDescriptors.size();

	for (int i = 0; i < numJoints; i++)
	{
		if (jointName == jointDescriptors[i].jointName)
			return &jointDescriptors[i];
	}

	return nullptr;
}

GameObject* HTRLoader::getGameObjectByName(std::string jointName)
{
	int numJoints = jointGameObjects.size();

	for (int i = 0; i < numJoints; i++)
	{
		if (jointName == jointGameObjects[i]->name)
			return jointGameObjects[i];
	}

	return nullptr;
}

void HTRLoader::createGameObjects()
{
	// Todo:
	// Create all of the GameObjects
	// Loop through each Joint descriptor and create game object for children
	for (int i = 0; i < jointDescriptors.size(); i++)
	{
		SkinnedGameObject* newJoint = new SkinnedGameObject();
        newJoint->name = jointDescriptors[i].jointName;
        newJoint->m_pJointAnimation = &jointDescriptors[i];

        newJoint->m_pJointAnimation->numFrames = jointDescriptors[i].numFrames;

		jointGameObjects.push_back(newJoint);
	}
	
	// Now that we have GameObjects for each joint, we need to set up
	// the kinematic linkage

	// Set up the hierarchy 
	for (int i = 0; i < jointDescriptors.size(); i++)
	{
		JointDescriptor* jd = &jointDescriptors[i];
		
		// Hint: use the getGameObjectByName function
        GameObject* child = getGameObjectByName(jd->jointName);
        GameObject* parent = getGameObjectByName(jd->jointParent);

		// Todo: Uncomment these lines
		if (child == nullptr || parent == nullptr)
			continue;

		parent->addChild(child);
	}

	
}

GameObject* HTRLoader::getRootGameObject()
{
	if (rootGameObject == nullptr)
	{
		for (int i = 0; i < jointGameObjects.size(); i++)
		{
			if (jointGameObjects[i]->m_pJointAnimation->jointParent == "GLOBAL")
				rootGameObject = jointGameObjects[i];
		}
	}
	return rootGameObject;
}

std::vector<SkinnedGameObject*> HTRLoader::getGameObjects()
{
	return jointGameObjects;
}

bool HTRLoader::processHeaderSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// File currently on line with "Header"
	// Go to the next line since it actually has data
	goToNextValidLineInFile(fp, buffer);

	// Keep processing headers until we make it to the next section
	// Once that happens, we will begin processing the new section
	do
	{
		// Allocate memory for values
		char identifierBuff[64];
		char valueBuff[64];

		// Split the current line (buffer)
		sscanf(buffer, "%s %s", identifierBuff, valueBuff);

		// Figure out which string identifier goes with which member variable in the class
		processHeader(std::string(identifierBuff), std::string(valueBuff));

		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// strstr returns null if second param isn't found in first
		loc = strstr(buffer, "SegmentNames");

	} while (loc == NULL);

	return true;
}

bool HTRLoader::processHeader(std::string identifier, std::string value)
{
	if (identifier == "FileType")
	{
		fileType = value;
		return true;
	}
	else if (identifier == "DataType")
	{
		dataType = value;
		return true;
	}
	else if (identifier == "FileVersion")
	{
		// remember fileVersion is an int
		// atoi converts a string to an int
		fileVersion = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "NumSegments")
	{
		numSegments = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "NumFrames")
	{
		numFrames = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "DataFrameRate")
	{
		fps = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "EulerRotationOrder")
	{
		if (value == "XYZ")
			rotationOrder = RotationOrder::XYZ;
		else if (value == "ZYX")
			rotationOrder = RotationOrder::ZYX;
		else
		{
			std::cout << "HTR Parse Warning: File has unsupported rotation order of: " << value << std::endl;
		}

		// put conditions for the other rotation orders here...
	}
	else if (identifier == "CalibrationUnits")
	{
		if (value == "mm")
		{
			calibrationUnits = CalibrationUnits::Millimeters;
			unitsScaleFactor = 0.1;
		}
		else if (value == "cm")
		{
			calibrationUnits = CalibrationUnits::Centimeters;
			unitsScaleFactor = 1.0f;
		}
		else
		{
			std::cout << "HTR Parse Warning: File has unsupported units: " << value << std::endl;
		}
	}
	else if (identifier == "RotationUnits")
	{
		rotationUnits = value;
	}
	else if (identifier == "GlobalAxisofGravity")
	{

	}
	else if (identifier == "BoneLengthAxis")
	{

	}
	else if (identifier == "ScaleFactor")
	{

	}
	else
	{
		std::cout << "HTR Parse Error: Could not identify HEADER: " << identifier << " with value: " << value << std::endl;
		return false;
	}

}

bool HTRLoader::processSegementNameSection(FILE * fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process segment names
	// File currently on line with "SegmentNames"
	goToNextValidLineInFile(fp, buffer);
	do
	{
		// Allocate memory for values
		char childBuff[64];
		char parentBuff[64];

		// Parse values from current line 
		sscanf(buffer, "%s %s", childBuff, parentBuff);

		// Create new joint descriptor
		JointDescriptor jd;

		// Set joint descriptor values
		jd.jointName = childBuff;
		jd.jointParent = parentBuff;

		// Store the new joint
		jointDescriptors.push_back(jd);

		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// strstr returns null if second param isn't found in first
		loc = strstr(buffer, "BasePosition");

	} while (loc == NULL);

	return true;
}

bool HTRLoader::processBasePositionSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process base positions
	// File currently on line with "BasePosition"
	// increment to next line with actual data
	goToNextValidLineInFile(fp, buffer);
	do
	{
		char jointNameBuff[64];
		float tx, ty, tz;
		float rx, ry, rz;
		float boneLength;

		// TODO:
		// 'buffer' contains the current line
        // Parse values from current line 
		// Parse this line and store the data in the temporary variables above
		
        // I'd recommend the "sscanf" function.
        sscanf(buffer, "%s %f %f %f %f %f %f %f", jointNameBuff, &tx, &ty, &tz, &rx, &ry, &rz, &boneLength);

		// Find the joint descriptor by name
		JointDescriptor* jd = getJointDescriptorByName(jointNameBuff);

		// Make sure we got it
		if (!jd)
		{
			std::cout << "HTR Parse ERROR: Could not find JointDescriptor with name: " << std::string(jointNameBuff) << std::endl;
			return false;
		}

		// store the parsed values in the appropriate locations
		jd->jointBasePosition = glm::vec3(tx, ty, tz) * unitsScaleFactor; // Note the scale factor multiplication. See 'unitsScaleFactor' definition for details.
		jd->jointBaseRotation = createQuaternion(rx, ry, rz);
		jd->boneLength = boneLength;

		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// strstr returns null if second param isn't found in first
		loc = strstr(buffer, jointDescriptors[0].jointName.c_str());
	} while (loc == NULL);
}

bool HTRLoader::processAnimationSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process each joint's poses at each frame
	for (int i = 0; i < jointDescriptors.size(); i++)
	{
		goToNextValidLineInFile(fp, buffer);
		do
		{

			// TODO: 
			// - create temporary variables to store the data
            int frameNumbers;
            float tx, ty, tz;
            float rx, ry, rz;
            float scale;
			// - parse the line (I recommend sscanf)

            sscanf(buffer, "%i %f %f %f %f %f %f %f", &frameNumbers, &tx, &ty, &tz, &rx, &ry, &rz, &scale);

			// - store the data in the appropriate location
            
            JointDescriptor* jd = &jointDescriptors[i];

            // Make sure we got it
            if (!jd)
            {
                std::cout << "HTR Parse ERROR: Could not find JointDescriptor with name: " << std::string(this->jointDescriptors[i].jointName) << std::endl;
                return false;
            }
            
            jd->numFrames = frameNumbers;
            jd->jointPositions.push_back(glm::vec3(tx, ty, tz) * unitsScaleFactor); // Note the scale factor multiplication. See 'unitsScaleFactor' definition for details.
            jd->jointRotations.push_back(createQuaternion(rx, ry, rz));
            jd->jointScales.push_back(scale);

			// Increment to next valid line
			goToNextValidLineInFile(fp, buffer);

			// Check if current line is the start of the next section
			// Remember strstr returns null if second param isn't found in first
			int nextJointIndex = i + 1;

			if (nextJointIndex < jointDescriptors.size())
				loc = strstr(buffer, jointDescriptors[nextJointIndex].jointName.c_str());
			else
			{
				//break;
				loc = strstr(buffer, "EndOfFile");
			}
		} while (loc == NULL);
	}

	return true;
}

void HTRLoader::goToNextValidLineInFile(FILE* fp, char* buffer)
{
	fgets(buffer, MAX_LINE_LENGTH, fp);

	// Move stream to next valid line
	// Remember: fgets will split on tabs
	// Remember: '#' is comment symbol
	while (buffer[0] == '\t' || buffer[0] == '#')
		fgets(buffer, MAX_LINE_LENGTH, fp);
}

glm::quat HTRLoader::createQuaternion(float rx, float ry, float rz)
{
	// If rotation units is degrees, convert to radians
	if (rotationUnits == "Degrees")
	{
		rx = glm::radians(rx);
		ry = glm::radians(ry);
		rz = glm::radians(rz);
	}

	// Construct unit quaternions for each axis
	// The reason why we create 3 quaternions for each axis is so we can control
	// the multiplication order.
	glm::quat qX = glm::angleAxis(rx, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qY = glm::angleAxis(ry, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qZ = glm::angleAxis(rz, glm::vec3(0.0f, 0.0f, 1.0f));

	// Check for rotation order
	if (rotationOrder == RotationOrder::XYZ)
		return qX * qY * qZ;
	else if (rotationOrder == RotationOrder::ZYX)
		return qZ * qY * qX;
	// ... put other rotation orders here ...
	else
	{
		std::cout << "HTR Parse Warning: File has unsupported rotation order" << std::endl;
	}

	// Return XYZ by default
	return qZ * qY * qX;
}