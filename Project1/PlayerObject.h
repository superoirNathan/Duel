#pragma once
#include "GameObject.h"
#include "controller.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class PlayerObject
{
public:
	PlayerObject();
	~PlayerObject();

	void Shoot(GameObject*, std::vector<GameObject*>, Input::Stick);
	void Death(GameObject*, GameObject*, std::vector<GameObject*>, std::vector<GameObject*>, int*);

	bool BulletMove = false;
	bool shouldBulletMove = true;
	bool once = false;
	bool CD = false;
//	int Bullet1BounceNum = 0, Bullet2BounceNum = 0, Bullet3BounceNum = 0, Bullet4BounceNum = 0, Bullet5BounceNum = 0;
	std::vector<float> BulletBounceNum;
	std::vector<glm::vec3> BulletMoveVal;

	int BulletNum = 0;
	//float Bullet1MoveX, Bullet1MoveZ;
	//float Bullet2MoveX, Bullet2MoveZ;
	//float Bullet3MoveX, Bullet3MoveZ;
	//float Bullet4MoveX, Bullet4MoveZ;
	//float Bullet5MoveX, Bullet5MoveZ;
	float PrevStickX, PrevStickY;
	float BulletCD = 0;
};