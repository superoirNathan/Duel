#include "PlayerObject.h"
#include <iostream>

PlayerObject::PlayerObject()
{
	numbOfBullets = 5;
	bulletCDLength = 1.0;

	for (int i = 0; i < numbOfBullets; i++)
		BulletBounceNum.push_back(0.f);

	for (int i = 0; i < numbOfBullets;i++)
		BulletMoveVal.push_back(glm::vec3(0.0f));

	for (int i = 0; i < numbOfBullets;i++) {
		bulletHitSheild.push_back(false);
		bulletHitPillar2.push_back(false);
        bulletAlive.push_back(false);

	}

}

PlayerObject::~PlayerObject()
{

}

void PlayerObject::Shoot(GameObject* Player, std::vector<GameObject*> Bullets, Input::Stick rStick)
{
    if (this->once == false || this->CD == true)
        return;

	Bullets[BulletNum]->translate = glm::translate(Bullets[BulletNum]->translate, Player->position); // This moves the Bullet to the Players Position
	Bullets[BulletNum]->position = glm::vec3(Bullets[BulletNum]->translate  * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));//set position of object
	this->BulletBounceNum[BulletNum] = 0;

	Bullets[BulletNum]->rotate = glm::rotate(Bullets[BulletNum]->rotate, Player->localRotation, glm::vec3(0.f, 1.f, 0.f)); // This rotates the Bullet to the same rotation as the player
	this->bulletHitSheild[BulletNum] = false;
	this->bulletHitPillar2[BulletNum] = false;
    bulletAlive[BulletNum] = true;

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
void PlayerObject::IterateBulletNum() {

	if (this->once == true && this->CD == false) // If once1 is true, bulletNum changes to the next num in line
	{
		if (this->BulletNum < this->BulletBounceNum.size() - 1)
			this->BulletNum += 1;
		else // If past the max bullet pool, player will take control of the first bullet
			this->BulletNum = 0;
		this->once = false;
		this->CD = true;
		this->shouldBulletPlaySound = true;
	}
}
void PlayerObject::BulletPillarCollision(std::vector<GameObject*> Bullets, GameObject& pillar1, GameObject& pillar2, GameObject& pillar3) {

	CheckBulletBounceLimit(Bullets, 3);

	for (int i = 0; i < Bullets.size(); i++) // change Bullets into function call variable
	{
		if (BulletBounceNum[i] <= 3 && (glm::length(BulletMoveVal[i]) != 0)) {

			if (glm::distance(Bullets[i]->position, pillar1.position) < pillar1.Radius)
			{
				float delta_x = Bullets[i]->position.x - pillar1.position.x;
				float delta_z = Bullets[i]->position.z - pillar1.position.z;
				float theta_radians = glm::atan(delta_z, delta_x);
				
				glm::vec2 temp = BulletMoveVal[i];
				float tempLength = glm::length(temp);

				BulletMoveVal[i].x = glm::cos(theta_radians) * 0.5;
				BulletMoveVal[i].y = glm::sin(theta_radians) * 0.5;

				float tempLengthNew = glm::length(BulletMoveVal[i]);

				BulletBounceNum[i]++;
				bulletHitSheild[i] = false;
				bulletHitPillar2[i] = false;
                shouldBulletBouncePlaySound = true;
			}
			if (bulletHitPillar2[i] == false) {
				if (glm::distance(Bullets[i]->position, pillar2.position) < pillar2.Radius)
				{
					float delta_x = Bullets[i]->position.x - pillar2.position.x;
					float delta_z = Bullets[i]->position.z - pillar2.position.z;
					float theta_radians = glm::atan(delta_z, delta_x);
					BulletMoveVal[i].x = glm::cos(theta_radians) * 0.5;
					BulletMoveVal[i].y = glm::sin(theta_radians) * 0.5;
					BulletBounceNum[i]++;
					bulletHitSheild[i] = false;
					bulletHitPillar2[i] = true;
                    shouldBulletBouncePlaySound = true;
				}
			}
			if (glm::distance(Bullets[i]->position, pillar3.position) < pillar3.Radius)
			{
				float delta_x = Bullets[i]->position.x - pillar3.position.x;
				float delta_z = Bullets[i]->position.z - pillar3.position.z;
				float theta_radians = glm::atan(delta_z, delta_x);
				BulletMoveVal[i].x = glm::cos(theta_radians) * 0.5;
				BulletMoveVal[i].y = glm::sin(theta_radians) * 0.5;
				BulletBounceNum[i]++;
				bulletHitSheild[i] = false;
				bulletHitPillar2[i] = false;
                shouldBulletBouncePlaySound = true;
			}
		}
	}

}
void PlayerObject::BulletOuterWallsCollision(std::vector<GameObject*> Bullets) {

	for (int i = 0; i < Bullets.size(); i++) // change Bullets into function call variable
	{
        if (glm::length(BulletMoveVal[i]) > 0) {
            if (Bullets[i]->position.x < -13.5 || Bullets[i]->position.x > 13.2) {
                BulletMoveVal[i].x *= -1, BulletBounceNum[i]++, bulletHitSheild[i] = false;bulletHitPillar2[i] = false;
                if (bulletAlive[i] = true)
                    shouldBulletBouncePlaySound = true;
            }
            if (Bullets[i]->position.z < -7.35 || Bullets[i]->position.z > 7.55) {
                BulletMoveVal[i].y *= -1, BulletBounceNum[i]++, bulletHitSheild[i] = false;bulletHitPillar2[i] = false;

                if (bulletAlive[i] = true)
                    shouldBulletBouncePlaySound = true;
            }
        }
	}
}


void PlayerObject::CheckBulletBounceLimit(std::vector<GameObject*> Bullets, int bounceLimit) {
	for (int i = 0; i < Bullets.size(); i++) // change p1Bullets into function call variable
	{
		if (BulletBounceNum[i] > bounceLimit)
		{
			BulletMoveVal[i].x = 0, BulletMoveVal[i].y = 0;
			Bullets[i]->translate = glm::translate(glm::mat4(), glm::vec3(-1000.0f, -100.0f, -1000.0f)); //set position of object
			Bullets[i]->position = glm::vec3(-1000.0f, -100.0f, -1000.0f); //set position of object

			BulletBounceNum[i] = 0;
			bulletHitSheild[i] = false;
			bulletHitPillar2[i] = false;
            bulletAlive[i] = false;
            shouldBulletBouncePlaySound = false;

		}
	}
}

void PlayerObject::ShieldCollision(PlayerObject& other, GameObject* Player, std::vector<GameObject*> otherBullets) {

	for (int i = 0; i < otherBullets.size(); i++)
	{
		if (other.bulletHitSheild[i] == false) {
			if (glm::distance(otherBullets[i]->position, Shield.position) < Shield.Radius)
			{
				//            std::cout << "P1Shield was Hit" << std::endl;
				other.BulletMoveVal[i] = glm::reflect(other.BulletMoveVal[i], glm::vec2(-glm::cos(Player->OppAngle), glm::sin(Player->OppAngle)));
				//            other.BulletMoveVal[i].y = glm::reflect(other.BulletMoveVal[i], glm::vec2(glm::cos(Player->OppAngle), glm::sin(Player->OppAngle))).y;
				other.BulletBounceNum[i]++;
				other.bulletHitSheild[i] = true; //if you hit the shield the bullet is garunteed to hit something else
				other.bulletHitPillar2[i] = false;
			}
		}
	}
}

void PlayerObject::Death(GameObject* Player1, GameObject* Player2, std::vector<GameObject*> p1Bullets, std::vector<GameObject*> p2Bullets, int& RoundWins, PlayerObject* Winner)
{
	// NOTE:: FIX SHIELD REINITIALIZTION 

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
		p1Bullets[i]->position.x = 1000;
		p1Bullets[i]->position.z = 1000;
		p2Bullets[i]->position.x = 1000;
		p2Bullets[i]->position.z = 1000;

		p1Bullets[i]->translate = glm::translate(glm::mat4(), p1Bullets[i]->position); //set position of object
		p2Bullets[i]->translate = glm::translate(glm::mat4(), p2Bullets[i]->position);
	}

	// Reinitalize Player2
	Player2->position = glm::vec3(8.f, 0.0f, 0.0f);
	Player2->translate = glm::translate(glm::mat4(), Player2->position); //set position of object
	Player2->rotate = glm::rotate(glm::mat4(), 1.5708f, glm::vec3(0.f, 1.f, 0.f));

	// Reinitalize Player1
	Player1->position = glm::vec3(-8.f, 0.0f, 0.0f);
	Player1->translate = glm::translate(glm::mat4(), Player1->position); //set position of object
	Player1->rotate = glm::rotate(glm::mat4(), 1.5708f, glm::vec3(0.f, 1.f, 0.f));
}
void PlayerObject::GetRightStick(GameObject* player, Input::Stick rStick, Input::Stick lStick) {
	// -------------------------------------------- RIGHT THUMB STICK --------------------------------------------
	// Get Opposite angle Player 2 is facing
	if (player->currAngle >= 0)
		player->OppAngle = (player->currAngle - glm::pi<float>());
	else
		player->OppAngle = (player->currAngle + glm::pi<float>());

	this->Shield.position.x = player->position.x + glm::cos(player->OppAngle) * 0.4f;
	this->Shield.position.z = player->position.z + glm::sin(player->OppAngle) * -0.4f;
	this->rStickAngle = (glm::atan(rStick.yAxis, rStick.xAxis) * (180.f / glm::pi<float>()) * float(0.0174533)); //Update rStickAngle value

																												 // Dead Zone Check
	if (sqrt(rStick.yAxis*rStick.yAxis + rStick.xAxis*rStick.xAxis) > 0.1f) // DeadZone 10%
	{
		if (player->currAngle > glm::pi<float>())
			player->currAngle -= 2 * glm::pi<float>();
		else if (player->currAngle < -glm::pi<float>())
			player->currAngle += 2 * glm::pi<float>();

		if (abs(player->currAngle - this->rStickAngle) >= 0.4f) // if difference between current angle and rStick angle is greater than 0.1f then...
		{
			if (player->currAngle < this->rStickAngle) // if current angle LESS than rStickAngle...
			{
				if (abs(player->currAngle - this->rStickAngle) < glm::pi<float>()) // if angle difference less than pi...
				{
					player->rotate = glm::rotate(player->rotate, +0.4f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle += 0.4f;
				}
				else // if angle difference greater than pi...
				{
					player->rotate = glm::rotate(player->rotate, -0.4f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle -= 0.4f;
				}
			}
			else // if current angle GREATER than rStickAngle...
			{
				if (abs(player->currAngle - this->rStickAngle) < glm::pi<float>())
				{
					player->rotate = glm::rotate(player->rotate, -0.4f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle -= 0.4f;
				}
				else
				{
					player->rotate = glm::rotate(player->rotate, +0.4f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle += 0.4f;
				}
			}
		}
		else if (abs(player->currAngle - this->rStickAngle) >= 0.1f)
		{
			if (player->currAngle < this->rStickAngle) // if current angle LESS than rStickAngle...
			{
				if (abs(player->currAngle - this->rStickAngle) < glm::pi<float>()) // if angle difference less than pi...
				{
					player->rotate = glm::rotate(player->rotate, +0.05f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle += 0.05f;
				}
				else // if angle difference greater than pi...
				{
					player->rotate = glm::rotate(player->rotate, -0.05f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle -= 0.05f;
				}
			}
			else // if current angle GREATER than rStickAngle...
			{
				if (abs(player->currAngle - this->rStickAngle) < glm::pi<float>())
				{
					player->rotate = glm::rotate(player->rotate, -0.05f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle -= 0.05f;
				}
				else
				{
					player->rotate = glm::rotate(player->rotate, +0.05f, glm::vec3(0.f, 1.f, 0.f));
					player->currAngle += 0.05f;
				}
			}
		}
	}
}

void PlayerObject::BulletCooldown(float deltaTime) {

	if (this->CD) // Bullet Cool Down
	{
		this->BulletCD += deltaTime;
		if (this->BulletCD > this->bulletCDLength)
		{
			this->BulletCD = 0;
			this->CD = false;
		}
	}
}

void PlayerObject::UpdatePrevStick(Input::Stick rStick)
{
	if (sqrt(rStick.yAxis*rStick.yAxis + rStick.xAxis*rStick.xAxis) > 0.1f) // If magnitude of rStick is greater than 10%, update prevStick
	{
		this->PrevStickX = rStick.xAxis;
		this->PrevStickY = rStick.yAxis;
	}
}
