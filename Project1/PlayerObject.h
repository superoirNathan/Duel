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
	void Death(GameObject*, GameObject*, std::vector<GameObject*>, std::vector<GameObject*>, int&, PlayerObject*);

	GameObject Shield;
	bool BulletMove = false;
	bool shouldBulletMove = true;
	bool once = false;
	bool CD = false;
	std::vector<float> BulletBounceNum;
	std::vector<glm::vec2> BulletMoveVal;

	int BulletNum = 0;
	int RoundWins = 0;
	float PrevStickX, PrevStickY;
	float BulletCD = 0;
};