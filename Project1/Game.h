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
	GameObject knight1, knight2, arena, pillars, knight1Animation,knight2Animation, bars, brick, slab, monkeyHeadLeft, monkeyHeadRight, bulletTemplate, P1RoundWins, P2RoundWins; //define objects in here.
    SkinnedGameObject skinnedGameObject1;

    //GameObject WinCircle; //left for debate
	std::vector<GameObject*> p1Bullets;
	std::vector<GameObject*> p2Bullets;
	std::shared_ptr<Mesh> Bullet;
	
    //particles
    std::vector<ParticleEmitter*> loadedEmitters; // loaded posiitons
    std::vector<ParticleEmitter> emitters;        // emitter positions
    std::vector<GameObject*> particleObj;         // drawn objects
	std::vector<GameObject*> UIP1Wins, UIP2Wins;
    GameObject emitterBase;                  // emitterOBJ Base Initializer
    void Game::ReadEmitterFile(std::string fileName);

    ShaderProgram passThrough;
	ShaderProgram phongNoTexture;
	ShaderProgram phong;
	ShaderProgram phongColorSides;
	ShaderProgram animationShader;
	ShaderProgram UI;
	std::vector<Light> pointLights;
	Light directionalLight;

	glm::mat4 cameraTransform;
	glm::mat4 cameraProjection;
	glm::mat4 cameraProjOrtho;
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
};