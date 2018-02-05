#pragma once
#include <glm\glm.hpp>
#include <vector>
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Light.h"
#include "Material.h"
#include "animation.h"
#include <memory>
#include "controller.h"
#include <glm/gtc/matrix_transform.hpp>

#include <GLM\gtx\transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <string>
#include "JointTypes.h"


class GameObject
{
public:
	GameObject();
	GameObject(std::shared_ptr<Mesh> m);
	~GameObject();

	Animation animation;
	std::shared_ptr<Mesh> mesh;
	glm::mat4 transform;
	glm::mat4 translate;
	glm::mat4 rotate;

	glm::vec3 position;
	float localRotation;
	float scale = 1;

	float Radius;
	float OppAngle;

    //--Skin Meshing--------------------------------------------------------------
    glm::vec3 m_pLocalScale;
    float globalScale = 1.0f;

    glm::vec3 m_pLocalRotationAngles; // local rotation angles (degrees)

    glm::vec3 m_pLocalPosition;
    glm::mat4 m_pLocalRotation;
    glm::mat4 m_pLocalTransformMatrix;
    glm::mat4 m_pLocalToWorldMatrix; // parentLocalToWorld * m_pLocalTransformation 

                                     // Forward kinematics
    GameObject* m_pParent; // Pointer to parent
    GameObject* m_pRoot; // Pointer to parent

    std::vector<GameObject*> m_pChildren; // Pointers to children

    // Forward Kinematics
    void addChild(GameObject* newChild);
    void removeChild(GameObject* rip);

    // Note: These now set LOCAL transformation properties
    // Local means: relative to the PARENT
    // Example:
    // if this gameobject's parent located at (0, 2, 0) in WORLD SPACE
    // and this gameobject's LOCAL position is (0,0,0) then that means it is (0,0,0) units
    // from its parent. The WORLD SPACE position of this gameobject would be (0,2,0).
    // If this gameobject's LOCAL position was (0,2,0) then it would be (0,2,0) units from
    // it's parent. Which would mean the gameobject's WORLD position would be (0,4,0).
    // (0,4,0) = world position of parent + gameobject's local offset from parent
    // This is a very important concept to understand!
    void setLocalPosition(glm::vec3 newPosition);
    void setLocalRotationAngleX(float newAngle);
    void setLocalRotationAngleY(float newAngle);
    void setLocalRotationAngleZ(float newAngle);
    void setLocalScale(glm::vec3 newScale);

    glm::vec3 getLocalPosition();
    glm::vec3 getLocalRotationAngles();
    glm::vec3 getLocalScale();

    // parentLocalToWorld * localTransformation
    glm::mat4 getLocalToWorldMatrix();

    glm::vec3 getWorldPosition();

    glm::mat4 getWorldRotation();

    std::vector<GameObject*> getChildren();

    GameObject* getParent();
    GameObject* getRoot();
    virtual void update(float dt);

    // Other Properties
    std::string name;
    glm::vec4 colour;
    float GameObject::getScaleFloat();
    void GameObject::setScaleFloat(float newScaleFloat);

    float deltaTime;

    //--Skin Meshing END--------------------------------------------------------------

	Material mat;

	void loadMesh(std::string meshFile);
	void loadTexture(TextureType type, std::string texFile);
	
	bool checkCollision(GameObject &other);
	bool checkBulletCollision(GameObject &other);

	void draw(ShaderProgram &shader,glm::mat4 cameraTransform, glm::mat4 cameraProjection, std::vector<Light> &pointLights, Light &directinalLight);

	void Movement(Input::Stick lStick);
};