#include "PlayerObject.h"

PlayerObject::PlayerObject()
{
	BulletBounceNum.push_back(0.f);
	BulletBounceNum.push_back(0.f);
	BulletBounceNum.push_back(0.f);
	BulletBounceNum.push_back(0.f);
	BulletBounceNum.push_back(0.f);

	BulletMoveVal.push_back(glm::vec3(0.0f));
	BulletMoveVal.push_back(glm::vec3(0.0f));
	BulletMoveVal.push_back(glm::vec3(0.0f));
	BulletMoveVal.push_back(glm::vec3(0.0f));
	BulletMoveVal.push_back(glm::vec3(0.0f));
}

PlayerObject::~PlayerObject()
{

}

void PlayerObject::Shoot(GameObject* Player, std::vector<GameObject*> Bullets, Input::Stick rStick)
{
	Bullets[BulletNum]->translate = glm::translate(Bullets[BulletNum]->translate, Player->position); // This moves the Bullet to the Players Position
	Bullets[BulletNum]->rotate = glm::rotate(Bullets[BulletNum]->rotate, Player->localRotation, glm::vec3(0.f, 1.f, 0.f)); // This rotates the Bullet to the same rotation as the player
	BulletMove = true; // This bool makes bullet still move after RT is done being pressed


		if (shouldBulletMove && sqrt(rStick.yAxis*rStick.yAxis + rStick.xAxis*rStick.xAxis) != 0.f) // Shoots Bullet in Direction the player is facing w/ the right stick
		{
			BulletMoveVal[BulletNum].x = 1.0 * glm::cos(((glm::atan(rStick.yAxis, rStick.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533))) * +0.5; // This moves the bullet based on the X angle of RS
			BulletMoveVal[BulletNum].y = 1.0 * glm::sin(((glm::atan(rStick.yAxis, rStick.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533))) * -0.5; // This moves the bullet based on the Y angle of RS
																																			 //shouldBulletMove = false;
		}
		if (sqrt(rStick.yAxis*rStick.yAxis + rStick.xAxis*rStick.xAxis) == 0.f) // Shoots Bullet if player chooses Direction inside the deadzone
		{
			BulletMoveVal[BulletNum].x = 1.0 * glm::cos(((glm::atan(PrevStickY, PrevStickX)) * (180.f / glm::pi<float>()) * float(0.0174533))) * +0.5;
			BulletMoveVal[BulletNum].y = 1.0 * glm::sin(((glm::atan(PrevStickY, PrevStickX)) * (180.f / glm::pi<float>()) * float(0.0174533))) * -0.5;
		}


}

void PlayerObject::Death(GameObject* Player1, GameObject* Player2, std::vector<GameObject*> p1Bullets, std::vector<GameObject*> p2Bullets, int& RoundWins, PlayerObject* Winner)
{
	// Player Round Win Point
	RoundWins++;
	// Reset All Bullets
	for (int i = 0; i < BulletBounceNum.size(); i++)
	{
		BulletBounceNum[i] = 0.0f;
		Winner->BulletBounceNum[i] = 0.0f;
	}

	for (int i = 0; i < BulletMoveVal.size(); i++)
	{
		BulletMoveVal[i].x = 0, BulletMoveVal[i].y = 0;
		Winner->BulletMoveVal[i].x = 0, Winner->BulletMoveVal[i].y = 0;
		p1Bullets[i]->position.x = 0;
		p1Bullets[i]->position.z = 0;
		p2Bullets[i]->position.x = 0;
		p2Bullets[i]->position.z = 0;

		p1Bullets[i]->translate = glm::translate(glm::mat4(), p1Bullets[i]->position); //set position of object
		p2Bullets[i]->translate = glm::translate(glm::mat4(), p2Bullets[i]->position);
	}

	// Reinitalize Player2
	Player2->position = glm::vec3(8.f, 0.0f, 0.0f);
	Player2->translate = glm::translate(glm::mat4(), Player2->position); //set position of object

	// Reinitalize Player1
	Player1->position = glm::vec3(-8.f, 0.0f, 0.0f);
	Player1->translate = glm::translate(glm::mat4(), Player1->position); //set position of object
}
