//Nathan Alphonse 100523109
//Janarthan Ravikumar 100555192

#include "SkinnedGameObject.h"
#include <GLM/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "HTRLoader.h"
#include <iostream>
#include <GL\glew.h>
#include <include\tinyxml2\tinyxml2.h>
//#include "TTK/GraphicsUtils.h"
#include "GLM/gtx/string_cast.hpp"
#include <math.h>

SkinnedGameObject::SkinnedGameObject()
    : m_pCurrentFrame(0),
    m_pHTRAnimation(nullptr),
    m_pJointAnimation(nullptr),
    m_pPlaying(true), m_pDrawMesh(true), m_pDrawBindPose(false), m_pXrayJoints(true),
    m_pJointToBindMat(glm::mat4(1.0f)),
    m_pJointToInvMat(glm::mat4(1.0f)),
    step(1.0f),
    curvePlaySpeed(1.0f),
    m_pJointToInvMatCalutated(false)
{
}

SkinnedGameObject::~SkinnedGameObject()
{
	delete m_pHTRAnimation;
	delete m_pJointAnimation;
}

void SkinnedGameObject::setXRayJoints(bool xray)
{
    m_pXrayJoints = xray;
    for (int i = 0; i < m_pChildren.size(); i++)
    {
        auto sg = dynamic_cast<SkinnedGameObject*>(m_pChildren[i]);
        if (sg)
            sg->setXRayJoints(m_pXrayJoints);
    }
}

void SkinnedGameObject::setFrameJoints(int frameNumb)
{
    m_pCurrentFrame = frameNumb;
}

void SkinnedGameObject::initializeSkeletonFromHTR(std::string htrFilePath, std::string skinWeightFilePath, std::shared_ptr<Mesh> bindMesh)
{
	// Load the HTR data
	m_pHTRAnimation = new HTRLoader();
	m_pHTRAnimation->loadHTR(htrFilePath); // TODO: IMPLEMENT THIS FUNCTION

	// Create GameObjects from the data
	m_pHTRAnimation->createGameObjects();  // TODO: IMPLEMENT THIS FUNCTION

	// Add the root node from the HTR file as a child to
	// this GameObject. This allows us to control the locomotion of the hierarchy
	// my changing 'this' GameObject's scale, rotation and translation

    addChild(m_pHTRAnimation->getRootGameObject());

    // The root object (this object) will update the animation playback, all children
    // will look to this object to see what the current animation frame is
    if(!reverse)
    {
        auto gameObjects = m_pHTRAnimation->getGameObjects();
        for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++)
        {
            (*itr)->m_pCurrentFrame = m_pCurrentFrame;
            (*itr)->keyframeController.addKey(glm::vec3(m_pCurrentFrame));
                step = (*itr)->m_pJointAnimation->numFrames;
            
            (*itr)->keyframeController.addKey(glm::vec3((*itr)->m_pJointAnimation->numFrames));


        }
    }


    // Load Skin data

    tinyxml2::XMLDocument skinWeightFile;
    skinWeightFile.LoadFile(skinWeightFilePath.c_str());

    tinyxml2::XMLElement* xmlRoot = skinWeightFile.FirstChildElement("deformerWeight");
    tinyxml2::XMLElement* xmlJointItr = xmlRoot->FirstChildElement("weights");

    // Loop through each joint
    for (; xmlJointItr != nullptr; xmlJointItr = xmlJointItr->NextSiblingElement())
    {
        // Get reference to the Joint data
        JointDescriptor* joint = m_pHTRAnimation->getJointDescriptorByName(xmlJointItr->Attribute("source"));
        if (joint == nullptr)
        {
            std::cout << "Error parsing vertex weights" << std::endl;
            assert(0);
        }

        // Load in which vertices are influenced by this joint
        tinyxml2::XMLElement* xmlWeightItr = xmlJointItr->FirstChildElement("point");
        for (; xmlWeightItr != nullptr; xmlWeightItr = xmlWeightItr->NextSiblingElement())
        {
            int vertexIdx = atoi(xmlWeightItr->Attribute("index"));
            float jointWeight = atof(xmlWeightItr->Attribute("value"));
            joint->skinWeights.push_back(std::pair<int, float>(vertexIdx, jointWeight));
        }
        //std::cout << xmlJointItr->Attribute("source") << std::endl;
    }

    m_pBindMesh = bindMesh;

    // Create an intermediate mesh for skinning
    // We're essentially going to generate a new OBJ mesh every frame
    mesh = std::make_shared<Mesh>();
    mesh->objVertices.resize(m_pBindMesh->objVertices.size());
    mesh->objNormals.resize(m_pBindMesh->objNormals.size());
    mesh->faceData.resize(m_pBindMesh->faceData.size());
    mesh->objUVs.resize(m_pBindMesh->objUVs.size());

    // Texture coordinates do not need to be morphed, so we can just initialize them here once
    memcpy(&mesh->objUVs[0], &m_pBindMesh->objUVs[0], mesh->objUVs.size() * sizeof(glm::vec2));

    // Copy the face layout into the new mesh
    memcpy(&mesh->faceData[0], &m_pBindMesh->faceData[0], mesh->faceData.size() * sizeof(MeshFace));

    // Tell all the children to use these meshes
    auto gameObjects = m_pHTRAnimation->getGameObjects();
    for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++)
    {
        (*itr)->mesh = mesh;
        (*itr)->m_pBindMesh = m_pBindMesh;
    }

}

glm::mat4 SkinnedGameObject::getJointToBindMatrix()
{
	return m_pJointToBindMat;
}

void SkinnedGameObject::drawMatrix() 
{
    // Root object will set up the intermediate skinning mesh
    if (m_pParent == nullptr)
    {
        // Clear existing vertices and normals in temporary mesh
        memset(&mesh->objVertices[0], 0, mesh->objVertices.size() * sizeof(glm::vec3));
        memset(&mesh->objNormals[0], 0, mesh->objNormals.size() * sizeof(glm::vec3));
    }
    if (m_pJointAnimation != nullptr)
    {
        // Tip: compute this once and store it somewhere
        // Computing the inverse of a matrix is an expensive operation!

        // TODO: calculate m_pJointToBindMat in the update function
        if (!m_pJointToInvMatCalutated) {
            m_pJointToInvMat = glm::inverse(m_pJointToBindMat);
            m_pJointToInvMatCalutated = true;
            std::cout << "NANI?!!!!" << std::endl;
        }
        // Loop through each vertex this joint influences
 //       for (auto itr = m_pJointAnimation->skinWeights.begin(); itr != m_pJointAnimation->skinWeights.end() - 1; itr++) 
            for (int i = 0; i < m_pJointAnimation->skinWeights.size(); i++)
            {
                auto itr = &m_pJointAnimation->skinWeights[i];
                int vertexIdx = (*itr).first;
                float jointWeight = (*itr).second;

                    // Vertex in the bind mesh
               
                glm::vec3 vBind = m_pBindMesh->objVertices[vertexIdx]; //Vi

                    glm::vec4 vJointSpace = m_pJointToInvMat * glm::vec4(vBind, 1.0f);//(Tj->Bind)^-1

                    glm::vec4 vWorldSpace = m_pLocalToWorldMatrix * vJointSpace; //(Tj->w)

                    glm::vec3 vSkinned = jointWeight * vWorldSpace; //Wji

                    mesh->objVertices[vertexIdx] += vSkinned; // summation j->n

                    // TODO: Update the normals
                    glm::vec3 nBind = m_pBindMesh->objNormals[vertexIdx];
                    glm::vec4 nJointSpace = m_pJointToInvMat * glm::vec4(nBind, 0.0f);

                    glm::vec4 nWorldSpace = m_pLocalToWorldMatrix * nJointSpace;

                    glm::vec3 nSkinned = jointWeight * nWorldSpace;

                    mesh->objNormals[vertexIdx] += (glm::normalize(nSkinned));
            }
    }
    // Draw children (each joint performs skinning on the vertices they influence)
    for (int i = 0; i < m_pChildren.size(); ++i)
            dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->drawMatrix();;

    // Only the root object will draw the final mesh
    if (m_pParent == nullptr)
    {
//        mesh->draw(glm::mat4(1.0f));
//        mesh = mesh;


        // UpdateMesh is a new function added to the OBJMesh class
        // Take a look at what it does.
        mesh->updateMesh(true, true, true, false);

    }
}

void SkinnedGameObject::checkUpIK()
{
    // Check children of IK Parent

    glm::vec3 boneLength;
    glm::vec3 DirVec;
    glm::vec3 temp;
    SkinnedGameObject* goUpChild = nullptr;
    SkinnedGameObject* goUpChildTemp = nullptr;

    for (int i = 0; i < m_pChildren.size(); i++) {

        if (dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->childIk) {
            //check if end of tree is a target child

            // fix the parents before they die
            goUpChild = dynamic_cast<SkinnedGameObject*>(m_pChildren[i]->getParent());

                //get boneLength Prior to attempt
                // bonelength = parentPos - childPos
                boneLength = glm::vec4(m_pChildren[i]->getLocalPosition(), 1.0f);

                // if it needs ik, snap it to target
                m_pChildren[i]->setLocalPosition(targetPos);


                DirVec = glm::normalize(-glm::vec4(m_pChildren[i]->getLocalPosition(), 1.0f));
                // save bonelength so we can get length before we change position 
                temp = boneLength;

                // Next bonelength = parent->parentPos - ParentPos
                // boneLength = goUpChild->m_pParent->getLocalToWorldMatrix() * glm::vec4(goUpChild->m_pParent->getLocalPosition(), 1.0f) - glm::vec4(goUpChild->m_pLocalPosition, 1.0f);

                boneLength = glm::vec4(goUpChild->m_pLocalPosition, 1.0f);
                //set the parent's position to a length relative to child
                goUpChild->setLocalPosition( DirVec *abs(temp) );
                {
                    //              if (goUpChild->m_pParent != nullptr)
                    //                  goUpChild = dynamic_cast<SkinnedGameObject*>(goUpChild->getParent());
                    //              else
                    //                  break;
                    //          }
                }
        }
        else {
            {
                goUpChildTemp = goUpChild;
                // fix until u reach parent
                while (goUpChild != nullptr && !(goUpChild->parentIK))
                {
                    // DirVec = parentPos - parentspace[M]* currentPos
                    temp = boneLength;

                    boneLength = glm::vec4(goUpChild->getLocalPosition(), 1.0f);

                    DirVec = glm::normalize(glm::vec4(m_pChildren[i]->getLocalPosition(), 1.0f));

                    goUpChild->m_pLocalPosition = DirVec *abs(temp);

                    goUpChild = dynamic_cast<SkinnedGameObject*>(goUpChild->getParent());

                }
            }
            goUpChild = goUpChildTemp;

        }


        //repeat until end of tree
        dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->checkUpIK();
        //        DirVec = glm::normalize(-glm::vec4(m_pChildren[i]->getLocalPosition(), 1.0f));

    }

   //     temp = boneLength;
   //     if(goUpChild != nullptr)
   //     goUpChild->m_pLocalPosition = DirVec *abs(temp);
   //
}

void SkinnedGameObject::update(float dt)
{
	// This GameObject has no jointAnimation so we will construct our
	// transformation matrices in the traditional way
	if (m_pJointAnimation == nullptr)
	{
        for (int i = 0; i < m_pChildren.size(); i++) {
            dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->targetPos = targetPos;
        }
		if (!m_pPlaying)
			dt = 0.0f;
		GameObject::update(dt);
	}
	else
	{
		/// Todo: create localRotation, scale and translation matrices using HTR data

        keyframeController.calculateLookupTable(step);
        keyframeController.curvePlaySpeed = 10/ (curvePlaySpeed + 2);
        keyframeController.doesSpeedControl = true;
        glm::vec3 temp = keyframeController.update(dt);
        float D = temp.x;
        m_pCurrentFrame = D;


        auto goUpChild = dynamic_cast<SkinnedGameObject*>(getParent());
        //        int i = 0;

        if (goUpChild->m_pParent ==nullptr && goUpChild->m_pRootPose) {
            setFrameJoints(0);
        }   


        while (goUpChild != nullptr)
        {
            if (goUpChild->m_pDrawBindPose) {
                setFrameJoints(0);
            }
            if (goUpChild->m_pParent != nullptr)
                goUpChild = dynamic_cast<SkinnedGameObject*>(goUpChild->getParent());
            else
                break;
        }
        if (goUpChild->reverse){
            m_pCurrentFrame = m_pJointAnimation->numFrames - 1 - D;
        }
        
        float floorFrame = ceilf(m_pCurrentFrame);

        m_pLocalRotation =
			glm::mat4_cast(m_pJointAnimation->jointBaseRotation *
				m_pJointAnimation->jointRotations[m_pCurrentFrame]);
    
		m_pLocalPosition = glm::vec3(m_pJointAnimation->jointBasePosition +
            m_pJointAnimation->jointPositions[m_pCurrentFrame]);
   
		m_pLocalScale = glm::vec3(m_pJointAnimation->jointScales[m_pCurrentFrame]);
        


        glm::mat4 floor_m_pLocalRotation =
            glm::mat4_cast(m_pJointAnimation->jointBaseRotation *
                m_pJointAnimation->jointRotations[floorFrame]);

        glm::vec3 floor_m_pLocalPosition = glm::vec3(m_pJointAnimation->jointBasePosition +
            m_pJointAnimation->jointPositions[floorFrame]);

        glm::vec3 floor_m_pLocalScale = glm::vec3(m_pJointAnimation->jointScales[floorFrame]);

        m_pLocalRotation = (m_pLocalRotation + floor_m_pLocalRotation) / 2.0f;
        if (deltaTime >= 1)
            deltaTime = 0;
        m_pLocalPosition = Math::lerp(m_pLocalPosition, floor_m_pLocalPosition, deltaTime);
        m_pLocalScale = Math::lerp(m_pLocalScale, floor_m_pLocalScale, deltaTime);


        curvePlaySpeed = goUpChild->curvePlaySpeed;

        //change local Position of children if you're a parent for IK
    //    if (goUpChild->seekTarget) {
    //        checkUpIK();
    //    }

		//// Create translation matrix
        glm::mat4 tran = glm::translate(m_pLocalPosition);
    
		//// Create scale matrix
        glm::mat4 scal = glm::scale(m_pLocalScale);
        if (seekTarget)
            m_pLocalTransformMatrix = tran *m_pLocalRotation * scal;
        else
            m_pLocalTransformMatrix = tran * m_pLocalRotation * scal;

        m_pJointToBindMat =  glm::translate( m_pJointAnimation->jointBasePosition) * glm::mat4_cast(m_pJointAnimation->jointBaseRotation);

		if (m_pParent)
        {
            m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix()* m_pLocalTransformMatrix;
            m_pJointToBindMat = ((SkinnedGameObject*)m_pParent)->getJointToBindMatrix()*m_pJointToBindMat;
		}
		else
		{
            m_pLocalToWorldMatrix = m_pLocalTransformMatrix;
		}

		// Update children
        for (int i = 0; i < m_pChildren.size(); i++) {

            dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->reverse = reverse;
            dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->curvePlaySpeed = curvePlaySpeed;
            dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->m_pDrawBindPose = m_pDrawBindPose;            
        }
        for (int i = 0; i < m_pChildren.size(); i++) {
            dynamic_cast<SkinnedGameObject*>(m_pChildren[i])->targetPos = targetPos;

            m_pChildren[i]->update(dt);
        }

		// Increment frame to next frame
		// Note: Right now we are just playing the animation back frame by frame
		// Since we exported the animation at 60 fps, as long as the game runs at 60 fps this works fine
		// However, to make this more robust, you should blend between frames (i.e. lerp / slerp) to get
		// frame rate independent playback. This will also allow you to control the
		// playback speed of the animation (hint: this is what you need to do for assignment 3!)
		//m_pCurrentFrame++;

		// Make sure we are within our animation frame count
		if ((m_pCurrentFrame) >= (int)m_pJointAnimation->numFrames)
			(m_pCurrentFrame) = 0;
	}
}