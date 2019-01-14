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
    void IterateBulletNum();
	void Death(GameObject*, GameObject*, std::vector<GameObject*>, std::vector<GameObject*>, int&, PlayerObject*);
    void BulletPillarCollision(std::vector<GameObject*> Bullets, GameObject& pillar1, GameObject& pillar2, GameObject& pillar3);
    void BulletOuterWallsCollision(std::vector<GameObject*> Bullets);
    void CheckBulletBounceLimit(std::vector<GameObject*> Bullets, int bounceLimit);
    void ShieldCollision(PlayerObject& other, GameObject* Player, std::vector<GameObject*> Bullets);
    void GetRightStick(GameObject* player, Input::Stick rStick, Input::Stick lStick);
    void BulletCooldown(float deltaTime);
    void UpdatePrevStick(Input::Stick);
	GameObject Shield;
	bool BulletMove = false;
	bool shouldBulletMove = true;
    bool shouldBulletPlaySound = false;
    bool shouldBulletBouncePlaySound = false;
    bool once = false;
	bool CD = false;
	std::vector<float> BulletBounceNum;
	std::vector<glm::vec2> BulletMoveVal;
    std::vector<bool> bulletHitSheild;
    std::vector<bool> bulletHitPillar2;
    std::vector<bool> bulletAlive;

//    std::vector<bool> bulleOffline;

	int BulletNum = 0;
	int RoundWins = 0;
    int numbOfBullets = 5;
	float PrevStickX, PrevStickY;
	float BulletCD = 0;
    float bulletCDLength = 1;
	float rStickAngle = 0;
};