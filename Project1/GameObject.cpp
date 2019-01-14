#include "GameObject.h"
#include <iostream>
#include "animation.h"
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject()
{
}
GameObject::GameObject(std::shared_ptr<Mesh> m)
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

bool GameObject::checkCollision(GameObject &other)
{
	glm::vec4 myPos = (translate * rotate * glm::scale(glm::mat4(), glm::vec3(scale))) * glm::vec4(0, 0, 0, 1);
	glm::vec4 otherPos = (other.translate * other.rotate * glm::scale(glm::mat4(), glm::vec3(other.scale))) * glm::vec4(0, 0, 0, 1);

	bool overlapsX = false, overlapsY = false, overlapsZ = false;

	// When perfoming this calculation, we assume that pos is at the centre of the object.
	overlapsX = abs(otherPos.x - myPos.x) < ((mesh->width / 2.f) + (other.mesh->width / 2.f));
	overlapsY = abs(otherPos.y - myPos.y) < ((mesh->height / 2.f) + (other.mesh->height / 2.f));
	overlapsZ = abs(otherPos.z - myPos.z) < ((mesh->depth / 2.f) + (other.mesh->depth / 2.f));

	return overlapsX && overlapsY && overlapsZ; // if any one of these fail than the bounding box is not colliding
}

bool GameObject::checkBulletCollision(GameObject &other) //Check Bullet and Shield Collision
{
	glm::vec4 myPos = (translate * rotate * glm::scale(glm::mat4(), glm::vec3(scale))) * glm::vec4(0, 0, 0, 1);
	glm::vec4 otherPos = (other.translate * other.rotate * glm::scale(glm::mat4(), glm::vec3(other.scale))) * glm::vec4(0, 0, 0, 1);

	bool overlapsX = false, overlapsZ = false;
	bool SAoverlapsX = false, SAoverlapsZ = false; // Seperate axis check

	// When perfoming this fcalculation, we assume that pos is at the centre of the object.
	overlapsX = abs(otherPos.x - myPos.x) < ((mesh->width / 2.f) + (other.mesh->width / 2.f));
	overlapsZ = abs(otherPos.z - myPos.z) < ((mesh->depth / 2.f) + (other.mesh->depth / 2.f));

	// (Check overlap on axis of diagonal hitbox)


	return overlapsX && overlapsZ && SAoverlapsX && SAoverlapsZ; // if any one of these fail than the bounding bos is not colliding
}

void GameObject::draw(ShaderProgram &shader,glm::mat4 cameraTransform, glm::mat4 cameraProjection, Light &pointLight, GameObject gm)
{
	shader.bind();
	shader.sendUniformMat4("uModel", glm::value_ptr(transform), false);
	shader.sendUniformMat4("uView", glm::value_ptr(cameraTransform), false);
	shader.sendUniformMat4("uProj", glm::value_ptr(cameraProjection), false);

	//texture
	shader.sendUniform("material.diffuse", 0); //0 because diffuse in 0 slot
	shader.sendUniform("material.specular", 1); //1 because specular in 1 slot
	shader.sendUniform("material.normal", 2); //2 because normal in 2 slot
    shader.sendUniform("material.texture", 3); 
    shader.sendUniform("material.warp", 4);
    shader.sendUniform("material.hue", mat.hue);
	shader.sendUniform("material.specularExponent", mat.specularExponent);
	shader.sendUniform("shaderNumber", gm.shaderNumber);

    std::string prefix = "pointLight.";

		shader.sendUniform(prefix + "position", cameraTransform * pointLight.positionOrDirection);
		shader.sendUniform(prefix + "ambient", pointLight.ambient);
		shader.sendUniform(prefix + "diffuse", pointLight.diffuse);
		shader.sendUniform(prefix + "specular", pointLight.specular);
		shader.sendUniform(prefix + "specularExponent", pointLight.specularExponent); //at 1 has doesnt multiply, blocky lighting
		shader.sendUniform(prefix + "constantAttenuation", pointLight.constantAttenuation); //at 0.1 kind of has a light ring coming out from the central light
		shader.sendUniform(prefix + "linearAttenuation", pointLight.linearAttenuation); //at 0.01 also has ring, a little different. 100 takes light away

		shader.sendUniform(prefix + "quadraticAttenuation", pointLight.quadraticAttenuation); // bigger number smaller light

	shader.sendUniform("Time", animation.t);
    shader.sendUniform("in_uvScroll", this->uvScroll);

	glActiveTexture(GL_TEXTURE0);
	mat.diffuse.bind();

	glActiveTexture(GL_TEXTURE1);
	mat.specular.bind();

	glActiveTexture(GL_TEXTURE2);
	mat.normal.bind();

    glActiveTexture(GL_TEXTURE3);
    mat.texture.bind();

 //   glActiveTexture(GL_TEXTURE4);
 //   mat.warp.bind();

	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh->getNumVertices());
	glBindVertexArray(GL_NONE);

  //  mat.warp.unbind();
  //  glActiveTexture(GL_TEXTURE3);
    mat.texture.unbind();
    glActiveTexture(GL_TEXTURE2);
    mat.normal.unbind();
	glActiveTexture(GL_TEXTURE1);
	mat.specular.unbind();
	glActiveTexture(GL_TEXTURE0);
	mat.diffuse.unbind();

	shader.unbind();
}
void GameObject::drawParticle(ShaderProgram &shader, glm::mat4 cameraTransform, glm::mat4 cameraProjection, Light &pointLight){
    shader.bind();
    shader.sendUniformMat4("uModel", glm::value_ptr(transform), false);
    shader.sendUniformMat4("uView", glm::value_ptr(cameraTransform), false);
    shader.sendUniformMat4("uProj", glm::value_ptr(cameraProjection), false);
    //FLoats
    
    shader.sendUniform("in_width", this->mesh->geoWidth);
    shader.sendUniform("in_height", this->mesh->geoHeight);
    shader.sendUniform("in_time", this->mesh->geoTime);
    
    shader.sendUniform("u_texelSize", glm::vec4(1.0f/(this->mesh->width), 1.0f/(this->mesh->height), 0.0f, 0.0f)); //window width and window height doesnt work here

    //texture
    shader.sendUniform("material.diffuse", 0); //0 because diffuse in 0 slot
    shader.sendUniform("material.specular", 1); //1 because specular in 1 slot
    shader.sendUniform("material.normal", 2); //2 because normal in 2 slot
    shader.sendUniform("material.texture", 3);
    shader.sendUniform("material.warp", 4);
    shader.sendUniform("material.hue", mat.hue);
    shader.sendUniform("material.specularExponent", mat.specularExponent);
 //   shader.sendUniform("Time", animation.t);
    //send texture materials
    glActiveTexture(GL_TEXTURE0);
    mat.diffuse.bind();
    glActiveTexture(GL_TEXTURE1);
    mat.specular.bind();
    glActiveTexture(GL_TEXTURE2);
    mat.normal.bind();
    glActiveTexture(GL_TEXTURE3);
    mat.texture.bind();
  //  glActiveTexture(GL_TEXTURE4);
  //  mat.warp.bind();


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
 
    glDepthMask(GL_FALSE);
    glBindVertexArray(mesh->vao);
    glDrawArrays(GL_POINTS, 0, mesh->getNumVertices()/3.0f); // equivilate to vbo.draw() in tutorials
    glBindVertexArray(GL_NONE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    //mat.warp.unbind();
    //glActiveTexture(GL_TEXTURE3);
    mat.texture.unbind();
    glActiveTexture(GL_TEXTURE2);
    mat.normal.unbind();
    glActiveTexture(GL_TEXTURE1);
    mat.specular.unbind();
    glActiveTexture(GL_TEXTURE0);
    mat.diffuse.unbind();

    shader.unbind();
}

void GameObject::Movement(Input::Stick lStick, GameObject& pillar1, GameObject& pillar2, GameObject& pillar3)
{
	//d::cout << this->position.x << "||" << this->position.z << std::endl;
	//d::cout << pillar1.position.x << "||" << pillar1.position.z << std::endl;

	// Before we move to next frame, lets store our pos before movement
	glm::mat4 preUpdateTranslate = translate;
	glm::vec3 preUpdatePos = position;

	// Now lets move the char regardless of pillar coliision

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
	// now if the new movement is within the pillar, lets undo the translation and put the idiot back in his place
	if (glm::distance(pillar1.position, this->position) < this->Radius + pillar1.Radius)
	{
		std::cout << "inside pillar 1" << std::endl;
		translate = preUpdateTranslate;
		position = preUpdatePos;
	}
	else if (glm::distance(pillar2.position, this->position) < this->Radius + pillar2.Radius)
	{
		std::cout << "inside pillar 2" << std::endl;
		translate = preUpdateTranslate;
		position = preUpdatePos;
	}
	else if (glm::distance(pillar3.position, this->position) < this->Radius + pillar3.Radius)
	{
		std::cout << "inside pillar3 " << std::endl;
		translate = preUpdateTranslate;
		position = preUpdatePos;
	}
	else
	{

		preUpdateTranslate = translate;
		preUpdatePos = position;

	}

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

	// now if the new movement is within the pillar, lets undo the translation and put the idiot back in his place
	if (glm::distance(pillar1.position, this->position) < this->Radius + pillar1.Radius)
	{
		std::cout << "inside pillar 1" << std::endl;
		translate = preUpdateTranslate;
		position = preUpdatePos;
	}
	else if (glm::distance(pillar2.position, this->position) < this->Radius + pillar2.Radius)
	{
		std::cout << "inside pillar 2" << std::endl;
		translate = preUpdateTranslate;
		position = preUpdatePos;
	}
	else if (glm::distance(pillar3.position, this->position) < this->Radius + pillar3.Radius)
	{
		std::cout << "inside pillar3 " << std::endl;
		translate = preUpdateTranslate;
		position = preUpdatePos;
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