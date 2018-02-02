#include "GameObject.h"
#include <iostream>
#include "animation.h"
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject()
    : m_pLocalScale(1.0f), m_pParent(nullptr), m_pRoot(this)
{

}
GameObject::GameObject(std::shared_ptr<Mesh> m)
    : m_pLocalScale(1.0f), m_pParent(nullptr), m_pRoot(this)
{
	mesh = m;
}
GameObject::~GameObject()
{
	//mesh.unload();
}
void GameObject::loadTexture(TextureType type, std::string texFile)
{
	mat.loadTexture(type, texFile);
}


void GameObject::draw(ShaderProgram &shader,glm::mat4 cameraTransform, glm::mat4 cameraProjection, std::vector<Light> &pointLights, Light &directionalLight)
{
	shader.bind();
	shader.sendUniformMat4("uModel", glm::value_ptr(transform), false);
	shader.sendUniformMat4("uView", glm::value_ptr(cameraTransform), false);
	shader.sendUniformMat4("uProj", glm::value_ptr(cameraProjection), false);

	//texture
	shader.sendUniform("material.diffuse", 0); //0 because diffuse in 0 slot
	shader.sendUniform("material.specular", 1); //1 because specular in 1 slot
	shader.sendUniform("material.normal", 2); //2 because normal in 2 slot
	shader.sendUniform("material.hue", mat.hue);
	shader.sendUniform("material.specularExponent", mat.specularExponent);

	for (int i = 0; i < pointLights.size(); i++)
	{
		std::string prefix = "pointLights[" + std::to_string(i) + "].";

		shader.sendUniform(prefix + "position", cameraTransform * pointLights[i].positionOrDirection);
		shader.sendUniform(prefix + "ambient", pointLights[i].ambient);
		shader.sendUniform(prefix + "diffuse", pointLights[i].diffuse);
		shader.sendUniform(prefix + "specular", pointLights[i].specular);
		shader.sendUniform(prefix + "specularExponent", pointLights[i].specularExponent); //at 1 has doesnt multiply, blocky lighting
		shader.sendUniform(prefix + "constantAttenuation", pointLights[i].constantAttenuation); //at 0.1 kind of has a light ring coming out from the central light
		shader.sendUniform(prefix + "linearAttenuation", pointLights[i].linearAttenuation); //at 0.01 also has ring, a little different. 100 takes light away
		shader.sendUniform(prefix + "quadraticAttenuation", pointLights[i].quadraticAttenuation); // bigger number smaller light
	}

	shader.sendUniform("directionalLight.direction", cameraTransform * directionalLight.positionOrDirection);
	shader.sendUniform("directionalLight.ambient", directionalLight.ambient);
	shader.sendUniform("directionalLight.diffuse", directionalLight.diffuse);
	shader.sendUniform("directionalLight.specular", directionalLight.specular);
	shader.sendUniform("directionalLight.specularExponent", directionalLight.specularExponent);


	shader.sendUniform("Time", animation.t);

	glActiveTexture(GL_TEXTURE0);
	mat.diffuse.bind();

	glActiveTexture(GL_TEXTURE1);
	mat.specular.bind();

	glActiveTexture(GL_TEXTURE2);
	mat.normal.bind();

	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh->getNumVertices());
	glBindVertexArray(GL_NONE);

	mat.normal.unbind();
	glActiveTexture(GL_TEXTURE1);
	mat.specular.unbind();
	glActiveTexture(GL_TEXTURE0);
	mat.diffuse.unbind();

	shader.unbind();
}

void GameObject::Movement(Input::Stick lStick)
{
	//if ((position.x + lStick.xAxis * 0.1 > 1.6 || position.x + lStick.xAxis * 0.1 < -1.6) || (position.z + lStick.yAxis * -0.1 > -2.9 || position.z + lStick.yAxis * -0.1 < -5.7)
	//	&& (position.z + lStick.yAxis * -0.1 > 1.4 || position.z + lStick.yAxis * -0.1 < -1.4) && (position.z + lStick.yAxis * -0.1 > 5.9 || position.z + lStick.yAxis * -0.1 < 2.9))
	{
		if (lStick.xAxis > 0 && position.x < 13) // Check Magnitude of Stick and
		{
			translate = glm::translate(translate, glm::vec3((lStick.xAxis) * 0.1, 0.f, 0.f));
			position.x += lStick.xAxis * 0.1; // Update local position for bullet
		}
		else if (lStick.xAxis < 0 && position.x > -13)
		{
			translate = glm::translate(translate, glm::vec3((lStick.xAxis) * 0.1, 0.f, 0.f));
			position.x += lStick.xAxis * 0.1; // Update local position for bullet
		}
	}
	if ((position.x + lStick.xAxis * 0.1 > 1.6 || position.x + lStick.xAxis * 0.1 < -1.6) || (position.z + lStick.yAxis * -0.1 > -2.8 || position.z + lStick.yAxis * -0.1 < -3)
		&& (position.z + lStick.yAxis * -0.1 > 1.5 || position.z + lStick.yAxis * -0.1 < 1.3) && (position.z + lStick.yAxis * -0.1 > 6 || position.z + lStick.yAxis * -0.1 < 5.8))
	{
		if ((position.x + lStick.xAxis * 0.1 > 1.6 || position.x + lStick.xAxis * 0.1 < -1.6) || (position.z + lStick.yAxis * -0.1 > -5.8 || position.z + lStick.yAxis * -0.1 < -6)
			&& (position.z + lStick.yAxis * -0.1 > -1.3 || position.z + lStick.yAxis * -0.1 < -1.5) && (position.z + lStick.yAxis * -0.1 > 3 || position.z + lStick.yAxis * -0.1 < 2.8))
		{
			if (lStick.yAxis > 0 && position.z > -7)
			{
				translate = glm::translate(translate, glm::vec3(0.f, 0.f, lStick.yAxis * -0.1));
				position.z += lStick.yAxis * -0.1; // Update local position for bullet
			}
			else if (lStick.yAxis < 0 && position.z < 7)
			{
				translate = glm::translate(translate, glm::vec3(0.f, 0.f, lStick.yAxis * -0.1));
				position.z += lStick.yAxis * -0.1; // Update local position for bullet
			}
		}
	}
}


void GameObject::addChild(GameObject * newChild)
{
    if (newChild)
    {
        m_pChildren.push_back(newChild);
        newChild->m_pParent = this; // tell new child that this game object is its parent
        if (m_pRoot != nullptr)
            newChild->m_pRoot = m_pRoot; // tell new child that it's root

    }
}

void GameObject::removeChild(GameObject * rip)
{
    for (int i = 0; i < m_pChildren.size(); ++i)
    {
        if (m_pChildren[i] == rip) // compare memory locations (pointers)
        {
            std::cout << "Removing child: " + rip->name << " from object: " << this->name;
            m_pChildren.erase(m_pChildren.begin() + i);
        }
    }
}

void GameObject::setLocalPosition(glm::vec3 newPosition)
{
    m_pLocalPosition = newPosition;
}

void GameObject::setLocalRotationAngleX(float newAngle)
{
    m_pLocalRotationAngles.x = newAngle;
}

void GameObject::setLocalRotationAngleY(float newAngle)
{
    m_pLocalRotationAngles.y = newAngle;
}

void GameObject::setLocalRotationAngleZ(float newAngle)
{
    m_pLocalRotationAngles.z = newAngle;
}

void GameObject::setLocalScale(glm::vec3 newScale)
{
    m_pLocalScale = newScale;
}

float GameObject::getScaleFloat() {
    return globalScale;
}
void GameObject::setScaleFloat(float newScaleFloat) {
    globalScale = newScaleFloat;
}

glm::vec3 GameObject::getLocalPosition()
{
    return m_pLocalPosition;
}

glm::vec3 GameObject::getLocalRotationAngles()
{
    return m_pLocalRotationAngles;
}

glm::vec3 GameObject::getLocalScale()
{
    return m_pLocalScale;
}

glm::mat4 GameObject::getLocalToWorldMatrix()
{
    return m_pLocalToWorldMatrix;
}

glm::vec3 GameObject::getWorldPosition()
{
    if (m_pParent)
        return m_pParent->getLocalToWorldMatrix() * glm::vec4(m_pLocalPosition, 1.0f);
    else
        return m_pLocalPosition;
}

glm::mat4 GameObject::getWorldRotation()
{
    if (m_pParent)
        return m_pParent->getWorldRotation() * m_pLocalRotation;
    else
        return m_pLocalRotation;
}

std::vector<GameObject*> GameObject::getChildren()
{
    return m_pChildren;
}

GameObject * GameObject::getParent()
{
    return m_pParent;
}

GameObject * GameObject::getRoot()
{
    return m_pRoot;
}

void GameObject::update(float dt)
{
    // Create 4x4 transformation matrix

    // Create rotation matrix
    glm::mat4 rx = glm::rotate(glm::radians(m_pLocalRotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ry = glm::rotate(glm::radians(m_pLocalRotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rz = glm::rotate(glm::radians(m_pLocalRotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Note: pay attention to rotation order, ZYX is not the same as XYZ
    m_pLocalRotation = rz * ry * rx;

    // Create translation matrix
    glm::mat4 tran = glm::translate(m_pLocalPosition);

    // Create scale matrix
    glm::mat4 scal = glm::scale(m_pLocalScale * globalScale);

    // Combine all above transforms into a single matrix
    // This is the local transformation matrix, ie. where is this game object relative to it's parent
    // If a game object has no parent (it is a root node) then its local transform is also it's global transform
    // If a game object has a parent, then we must apply the parent's transform
    m_pLocalTransformMatrix = tran * m_pLocalRotation * scal;

    if (m_pParent)
        m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix;
    else
        m_pLocalToWorldMatrix = m_pLocalTransformMatrix;

    // Update children
    for (int i = 0; i < m_pChildren.size(); i++)
        m_pChildren[i]->update(dt);
}



