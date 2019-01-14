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
#include "FrameBufferObject.h"
#include <vector>
#include <memory>

#include "SoundEngine.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define FRAMES_PER_SECOND 60

class Game
{
public:
	Game();
	~Game();

	enum GameState {PressStart, MainMenu, GameLoop, GameOver, loading};
	GameState gs = loading;
    
    // Fmod var
    SoundEngine se;
    float songTransitionDelay;
    // core functions
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
	float totalGameTime = 0;
	float menuButtonDelay = 0;
	float GameOverDelay = 0;
	glm::mat4 rightOffset = glm::rotate(glm::mat4(), 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 leftOffset = glm::rotate(glm::mat4(), -1.5708f, glm::vec3(0.f, 1.f, 0.f));

    //FBO
    FrameBufferObject fboScene, fboBrightPass, fboBlur, fboHold;
    //FBO QUAD
    GameObject quad;
    std::shared_ptr<Mesh> emptyQuadMesh;
    glm::vec3 quadPos;


    //Game Obj
	PlayerObject P1, P2;
	GameObject knight1, knight2, arena, pillar1, pillar2, pillar3, knight1Animation,knight2Animation, bars, brick, slab, monkeyHeadLeft, monkeyHeadRight, bulletTemplate, bulletTemplate2, P1RoundWins, P2RoundWins, mainmenu, credits; //define objects in here.
    //SkinMesh
    SkinnedGameObject skinnedGameObject1;                                                                                                                //GameObject WinCircle; //left for debate
	std::vector<GameObject*> p1Bullets;
	std::vector<GameObject*> p2Bullets;
	std::shared_ptr<Mesh> Bullet;
	
    //particle Loader
    std::vector<ParticleEmitter*> loadedEmitters; // loaded posiitons
    std::vector<ParticleEmitter> emitters;        // emitter positions
    //std::vector<GameObject*> particleObj;         // drawn objects
    //particle Geo and Pos
    GameObject partPosMeshAcess;
    std::shared_ptr<Mesh> emptyParticle;
    std::vector<glm::vec3*> particlePos;
    //UI + Menue
	std::vector<GameObject*>  menuState;

    //P1 QUADS
    GameObject P1Health;
    std::shared_ptr<Mesh> emptyP1HealthMesh;
    GameObject P1HealthIcons;
    std::shared_ptr<Mesh> emptyP1HealthIcons;
    std::vector<glm::vec3*> P1IconsPos;
    std::vector<glm::vec3> P1IconsOriginalPos;

    //P2 QUADS
    GameObject P2Health;
    std::shared_ptr<Mesh> emptyP2HealthMesh;
    GameObject P2HealthIcons;
    std::shared_ptr<Mesh> emptyP2HealthIcons;
    std::vector<glm::vec3*> P2IconsPos;
    std::vector<glm::vec3> P2IconsOriginalPos;

    GameObject emitterBase;                  // emitterOBJ Base Initializer
    void Game::ReadEmitterFile(std::string fileName);

    // Bullet Trails                            // the goal is to have each bullet have 1 emmiter and 1 trail
    std::vector<GameObject*> trailEmitterBase1, trailEmitterBase2;
    std::shared_ptr<Mesh> emptytrailMeshP1_1, emptytrailMeshP1_2, emptytrailMeshP1_3, emptytrailMeshP1_4, emptytrailMeshP1_5;
    std::shared_ptr<Mesh> emptytrailMeshP2_1, emptytrailMeshP2_2, emptytrailMeshP2_3, emptytrailMeshP2_4, emptytrailMeshP2_5;
    
    std::vector<ParticleEmitter> trailEmitter1, trailEmitter2;
    std::vector< std::vector<glm::vec3*> > trailEmitterPosHOLDER, trailEmitterPos2HOLDER2;

    ShaderProgram passThroughParticles;      // for FBO
	ShaderProgram passthrough;
	ShaderProgram phong;            // for basic lighting
    ShaderProgram phongUvScroll;    // for uvScroll
    ShaderProgram animationShader;  // for morph targets
	ShaderProgram UI;               // ortho perspective
    ShaderProgram geo;              // includes geometry shader
    ShaderProgram UI_geo;           // ortho perspective
    Light pointLight; 
    
    ShaderProgram gaussBlurr, bright, composite, postProcessing; //postProcess

	glm::mat4 cameraTransform;
	glm::mat4 cameraProjection;
	glm::mat4 cameraProjOrtho;
	glm::mat4 lightSpinner;

    glm::mat4 tempProjOrtho;
    glm::mat4 tempTransform;
private:
	bool playOrQuit = false;
	bool p1Orp2Win = false;

	bool PlayerOneWin = false;
	bool PlayerTwoWin = false;

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

	bool toggleTexture = false;
	bool freeze = false;
	float freezeTimer = 0;
	bool freezeP1 = false;
	bool freezeP2 = false;
};