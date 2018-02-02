#pragma once
#include "Timer.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "GameObject.h"
#include "PlayerObject.h"
#include "Texture.h"
#include "Light.h"
#include "ParticleEmitter.h"
#include "SkinnedGameObject.h"
#include <vector>
#include <memory>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define FRAMES_PER_SECOND 60

class Game
{
public:
	Game();
	~Game();

	void initializeGame();
	void update();
	void draw();

	//Input callback fuctions
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);

	//Date Memmber
	Timer* updateTime = nullptr;
	float totalGameTime = 0.f;

	PlayerObject P1, P2;
	GameObject knight1, knight2, arena, pillars, knight1Animation,knight2Animation, bars, brick, slab, monkeyHeadLeft, monkeyHeadRight, bulletTemplate; //define objects in here.
    SkinnedGameObject skinnedGameObject1;
	std::vector<GameObject*> p1Bullets;
	std::vector<GameObject*> p2Bullets;
	std::shared_ptr<Mesh> Bullet;
	
    //particles
    std::vector<ParticleEmitter*> loadedEmitters; // loaded posiitons
    std::vector<ParticleEmitter> emitters;        // emitter positions
    std::vector<GameObject*> particleObj;         // drawn objects
    GameObject emitterBase;                  // emitterOBJ Base Initializer
    void Game::ReadEmitterFile(std::string fileName);

    ShaderProgram passThrough;
	ShaderProgram phongNoTexture;
	ShaderProgram phong;
	ShaderProgram phongColorSides;
	ShaderProgram animationShader;
	std::vector<Light> pointLights;
	Light directionalLight;

	glm::mat4 cameraTransform;
	glm::mat4 cameraProjection;
	glm::mat4 lightSpinner;

private:
	bool shouldRotate = false;
	bool shouldLightsSpin = false;
	bool wKeydown = false;
	bool aKeydown = false;
	bool sKeydown = false;
	bool dKeydown = false;
	bool oKeydown = false;
	bool pKeydown = false;

	//camera move controls
	bool iKeyDown = false;
	bool kKeyDown = false;
	//camera projection controls
	bool uKeyDown = false;
	bool jKeyDown = false;

	bool doDeCast = false;
	bool doParenting = false;
	int p1RoundWins = 0;
	int p2RoundWins = 0;

	// Player Properties

	/*bool Bullet1Move = false;
	bool shouldBullet1Move = true;
	bool once1 = false;
	bool p1CD = false;
	int p1Bullet1BounceNum = 0, p1Bullet2BounceNum = 0, p1Bullet3BounceNum = 0, p1Bullet4BounceNum = 0, p1Bullet5BounceNum = 0;
	float p1Bullet1MoveX, p1Bullet1MoveZ;
	float p1Bullet2MoveX, p1Bullet2MoveZ;
	float p1Bullet3MoveX, p1Bullet3MoveZ;
	float p1Bullet4MoveX, p1Bullet4MoveZ;
	float p1Bullet5MoveX, p1Bullet5MoveZ;
	float p1PrevStickX;
	float p1PrevStickY;
	float p1BulletCD = 0;
	int p1BulletNum = 0;

	bool Bullet2Move = false;
	bool shouldBullet2Move = true;
	bool once2 = false;
	bool p2CD = false;
	int p2Bullet1BounceNum = 0, p2Bullet2BounceNum = 0, p2Bullet3BounceNum = 0, p2Bullet4BounceNum = 0, p2Bullet5BounceNum = 0;
	float p2Bullet1MoveX, p2Bullet1MoveZ;
	float p2Bullet2MoveX, p2Bullet2MoveZ;
	float p2Bullet3MoveX, p2Bullet3MoveZ;
	float p2Bullet4MoveX, p2Bullet4MoveZ;
	float p2Bullet5MoveX, p2Bullet5MoveZ;
	float p2PrevStickX;
	float p2PrevStickY;
	float p2BulletCD = 0;
	int p2BulletNum = 0;*/
};