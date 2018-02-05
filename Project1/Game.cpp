#include "Game.h"
#include "controller.h"
#include <iostream>
#include <fstream>  // added
#include <Windows.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FMOD\fmod.hpp>
#include "SoundSystem.h"
#include "animationMath.h"
#include "SkinnedGameObject.h"

FMOD::Channel *channel1 = nullptr;
FMOD::Channel *channel2 = nullptr;
FMOD::Sound *sound1 = nullptr;
FMOD::Sound *sound2 = nullptr;
FMOD::System *System;

Game::Game()
{

}
Game::~Game()
{
	delete updateTime;

	passThrough.unload();
}
float splineTime = 0;
//happens once at the beginning of the game
void Game::initializeGame()
{
	updateTime = new Timer();

	glEnable(GL_DEPTH_TEST);

	//FMOD
	std::string sounds[3] = {
		"sound1",
		"sound2",
		"sound3"
	};
	if (FMOD::System_Create(&System) != FMOD_OK)
		throw;

	int driverCount = 0;
	System->getNumDrivers(&driverCount);
	if (driverCount == 0)
		throw;

	System->init(32, FMOD_INIT_NORMAL, nullptr);

	//Create the Sounds *Doesn't play the sound*
	System->createSound("Sounds/POKEMANZ.mp3", FMOD_DEFAULT, 0, &sound1);
	System->createSound("Sounds/Sound_Shot.mp3", FMOD_DEFAULT, 0, &sound2);
	
	//Set the mode of the sound, not nessasary
	sound1->setMode(FMOD_LOOP_NORMAL); //No Loop
	sound2->setMode(FMOD_LOOP_OFF); //Loops
	
	//Plays the sounds
	System->playSound(sound1, nullptr, false, &channel1); //For BG music shot sound ->playSound inside button switch

	//Update *System for sounds
	System->update();
	//std::cout << sounds[sound1] << std::endl;

	//lights
	Light light1, light2;
	light1.positionOrDirection = glm::vec4(0.f, 30.f, 0.f, 1.f);
	light1.originalPosition = glm::vec4(0.f, 4.f, 0.f, 1.f);
	light1.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	light1.diffuse = glm::vec3(168.f / 255.f, 148.f / 255.f, 87.f / 255.f);
	light1.specular = glm::vec3(1.f, 1.f, 1.f);
	light1.specularExponent = 50.f;
	light1.constantAttenuation = 0.1f;
	light1.linearAttenuation = 0.01f;
	light1.quadraticAttenuation = 0.001f;

	light2.positionOrDirection = glm::vec4(0.f, -10.f, 0.f, 1.f);
	light2.originalPosition = glm::vec4(0.f, 4.f, 0.f, 1.f);
	light2.ambient = glm::vec3(0.05f, 0.05f, 0.05f); //rgb channels
	light2.diffuse = glm::vec3(0.7f, 0.7f, 0.7f); //rgb channels
	light2.specular = glm::vec3(1.f, 1.f, 1.f); //rgb channels
	light2.specularExponent = 50.f;
	light2.constantAttenuation = 1.f;
	light2.linearAttenuation = 0.1f;
	light2.quadraticAttenuation = 0.01f;

	//Add Lights to vector
	pointLights.push_back(light1);
	pointLights.push_back(light2);

	
	//Load Shaders
	if (!passThrough.load("shaders/passthrough.vert", "shaders/passthrough.frag"))
	{
		std::cout << "Shaders failed to initialzed" << std::endl;
		system("pause");
		exit(0);
	}
	if (!phongNoTexture.load("shaders/Phong.vert", "shaders/PhongNoTexture.frag"))	//Used for object with 
	{																				//with no current texture
		std::cout << "Shaders failed to initialzed" << std::endl;
		system("pause");
		exit(0);
	}
	if (!phong.load("shaders/Phong.vert", "shaders/Phong.frag"))
	{
		std::cout << "Shaders failed to initialzed" << std::endl;
		system("pause");
		exit(0);
	}
	if (!phongColorSides.load("shaders/PhongColorSides.vert", "shaders/PhongColorSides.frag"))	//Colouring individual sides
	{																							//of a cube
		std::cout << "Shaders failed to initialzed" << std::endl;
		system("pause");
		exit(0);
	}
	if (!animationShader.load("shaders/Animating.vert", "shaders/Phong.frag"))	//Colouring individual sides
	{																							//of a cube
		std::cout << "Animating shader failed to initialzed" << std::endl;
		system("pause");
		exit(0);
	}
	if (!UI.load("shaders/Phong.vert", "shaders/UI.frag"))
	{
		std::cout << "UI shader failed to initalize" << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________KNIGHT MESH ANIMATION 1 MESH INTIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> knight1Walk_1 = std::make_shared<Mesh>();
	if (!knight1Walk_1->loadFromFile("meshes/knight5.2_000001.obj")) {
		std::cout << "Cube 1 Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	std::shared_ptr<Mesh> knight1Walk_2 = std::make_shared<Mesh>();
	if (!knight1Walk_2->loadFromFile("meshes/knight5.2_000004.obj")) {
		std::cout << "Cube 3 Model 3 fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	std::shared_ptr<Mesh> knight1Walk_3 = std::make_shared<Mesh>();
	if (!knight1Walk_3->loadFromFile("meshes/knight5.2_000008.obj")) {
		std::cout << "Cube 3 Model 3 fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	std::shared_ptr<Mesh> knight1Walk_4 = std::make_shared<Mesh>();
	if (!knight1Walk_4->loadFromFile("meshes/knight5.2_000012.obj")) {
		std::cout << "Cube 3 Model 3 fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	std::shared_ptr<Mesh> knight1Walk_5 = std::make_shared<Mesh>();
	if (!knight1Walk_5->loadFromFile("meshes/knight5.2_000016.obj")) {
		std::cout << "Cube 3 Model 3 fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	std::shared_ptr<Mesh> knight1Walk_6 = std::make_shared<Mesh>();
	if (!knight1Walk_6->loadFromFile("meshes/knight5.2_000020.obj")) {
		std::cout << "Cube 3 Model 3 fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________ARENA MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> Arena = std::make_shared<Mesh>();
	if (!Arena->loadFromFile("meshes/theFinalArena.obj"))
	{
		std::cout << "Arena Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________PILLARS MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> Pillars = std::make_shared<Mesh>();
	if (!Pillars->loadFromFile("meshes/Pillars.obj"))
	{
		std::cout << "Pillar Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________BARS MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> Bars = std::make_shared<Mesh>();
	if (!Bars->loadFromFile("meshes/Bars.obj"))
	{
		std::cout << "Bars Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________BRICK MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> Brick = std::make_shared<Mesh>();
	if (!Brick->loadFromFile("meshes/Brick.obj"))
	{
		std::cout << "Brick Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________SLAB MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> Slab = std::make_shared<Mesh>();
	if (!Slab->loadFromFile("meshes/Slab.obj"))
	{
		std::cout << "Slab Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	// ______________________________________________________________________________BULLET MESH INITIALIZE______________________________________________________________________________
	Bullet = std::make_shared<Mesh>();
	if (!Bullet->loadFromFile("meshes/NewBullet.obj"))
	{
		std::cout << "Bullet Model fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	// ______________________________________________________________________________KNIGHT MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> knight = std::make_shared<Mesh>(); // Define objs that are used
	if (!knight->loadFromFile("meshes/KnightV4.obj"))
	{
		std::cout << "Knight Model fail to load." << std::endl;
		system("pause");
		exit(0);
	}
	// ______________________________________________________________________________MONKEY HEAD LEFT MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> MonkeyHeadLeft = std::make_shared<Mesh>();
	if (!MonkeyHeadLeft->loadFromFile("meshes/monkeyHeadLeft.obj"))
	{
		std::cout << "Monkey Head fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
	// ______________________________________________________________________________MONKEY HEAD RIGHT MESH INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> MonkeyHeadRight = std::make_shared<Mesh>();
	if (!MonkeyHeadRight->loadFromFile("meshes/monkeyHeadRight.obj"))
	{
		std::cout << "Monkey Haed fail to load. " << std::endl;
		system("pause");
		exit(0);
	}
    // ______________________________________________________________________________PARTICLE PLACEHOLDER MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> partObj = std::make_shared<Mesh>();
    if (!partObj->loadFromFile("meshes/particle.obj"))
    {
        std::cout << "Model failed to load" << std::endl;
        system("pause");
        exit(0);
    }
	//______________________________________________________________________________UI OBJECT INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> UIObj = std::make_shared<Mesh>();
	if (!UIObj->loadFromFile("meshes/Wins.obj"))
	{
		std::cout << "UI Object failed to load" << std::endl;
		system("pause");
		exit(0);
	}
	//______________________________________________________________________________SHIELD INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> ShieldObj = std::make_shared<Mesh>();
	if (!ShieldObj->loadFromFile("meshes/Shield.obj"))
	{
		std::cout << "Shield Object failed to load" << std::endl;
		system("pause");
		exit(0);
	}
    //______________________________________________________________________________Skin Meshing__________________________________________________________________________________________________
    std::shared_ptr<Mesh> SkinnedMesh = std::make_shared<Mesh>();;
    std::string animationPath = "animations/";
    std::string modelsPath = "meshes/";
    if (!SkinnedMesh->loadFromFile("meshes/guy.obj"))
    {
        std::cout << "Model failed to load" << std::endl;
        system("pause");
        exit(0);
    }
    // mesh->loadFromFile(modelsPath + "guy.obj");

	//--- @CUBE INITIALIZE ---//	
	knight1Animation.animation.keyFrames.push_back(knight1Walk_1);
	knight1Animation.animation.keyFrames.push_back(knight1Walk_2);
	knight1Animation.animation.keyFrames.push_back(knight1Walk_3);
	knight1Animation.animation.keyFrames.push_back(knight1Walk_4);
	knight1Animation.animation.keyFrames.push_back(knight1Walk_5);
	knight1Animation.animation.keyFrames.push_back(knight1Walk_6);

	// -------------------------------------------- KNIGHT_1 ANIMATION INITIALIZE --------------------------------------------
	knight1Animation.animation.initialize();
	knight1Animation.loadTexture(TextureType::Diffuse, "textures/KnightV5.png");
	knight1Animation.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	knight1Animation.position = glm::vec3(-8.f, 0.0f, 0.0f);
	knight1Animation.scale = 0.2f;
	knight1Animation.translate = glm::translate(knight1Animation.translate, knight1Animation.position);
	knight1Animation.localRotation = 1.5708f; //This is 90 degrees in radians
	knight1Animation.rotate = glm::rotate(glm::mat4(), knight1Animation.localRotation, glm::vec3(0.f, 1.f, 0.f));

	// -------------------------------------------- KNIGHT_1 INITIALIZE --------------------------------------------
	knight1 = GameObject(knight); //set the object to and OBJ 
	knight1.loadTexture(TextureType::Diffuse, "textures/KnightV5.png");
	knight1.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	knight1.position = glm::vec3(-8.f, 0.0f, 0.0f);
	knight1.translate = glm::translate(knight1.translate, knight1.position); //set position of object
	knight1.localRotation = 1.5708f; //This is 90 degrees in radians
	knight1.rotate = glm::rotate(glm::mat4(), knight1Animation.localRotation, glm::vec3(0.f, 1.f, 0.f));
	knight1.Radius = 1; //USE FOR CIRCLE COLLISIONS
	knight1.OppAngle = glm::pi<float>();

	// -------------------------------------------- SHIELD INITIALIZE --------------------------------------------
	P1.Shield = GameObject(ShieldObj);
	P1.Shield.loadTexture(TextureType::Diffuse, "textures/jonWShieldTexture.png");
	P1.Shield.position = knight1.position;
	P1.Shield.translate = glm::translate(glm::mat4(), P1.Shield.position);
	P1.Shield.Radius = 1.4f;

	P2.Shield = GameObject(ShieldObj);
	P2.Shield.loadTexture(TextureType::Diffuse, "textures/jonWShieldTexture.png");
	P2.Shield.position = knight1.position;
	P2.Shield.translate = glm::translate(glm::mat4(), P2.Shield.position);
	P2.Shield.Radius = 1.4f;

	// -------------------------------------------- KNIGHT_2 INITIALIZE --------------------------------------------
	knight2 = GameObject(knight); //set the object to and OBJ 
	knight2.loadTexture(TextureType::Diffuse, "textures/KnightB.png");
	knight2.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	knight2.position = glm::vec3(8.f, 0.0f, 0.0f);
	knight2.translate = glm::translate(knight2.translate, knight2.position); //set position of object
	knight2.localRotation = 1.5708f; //This is 90 degrees in radians
	knight2.rotate = glm::rotate(glm::mat4(), knight2.localRotation, glm::vec3(0.f, 1.f, 0.f));
	knight2.Radius = 1; //USE FOR CIRCLE COLLISIONS
	knight2.OppAngle = 0.f;

	// -------------------------------------------- BULLET INITIALIZE --------------------------------------------
	bulletTemplate.mesh = Bullet;
	bulletTemplate.loadTexture(TextureType::Specular, "textures/fullSpecular.png");

	p1Bullets.push_back(new GameObject(bulletTemplate));
	p1Bullets.push_back(new GameObject(bulletTemplate));
	p1Bullets.push_back(new GameObject(bulletTemplate));
	p1Bullets.push_back(new GameObject(bulletTemplate));
	p1Bullets.push_back(new GameObject(bulletTemplate));

	p2Bullets.push_back(new GameObject(bulletTemplate));
	p2Bullets.push_back(new GameObject(bulletTemplate));
	p2Bullets.push_back(new GameObject(bulletTemplate));
	p2Bullets.push_back(new GameObject(bulletTemplate));
	p2Bullets.push_back(new GameObject(bulletTemplate));

	// -------------------------------------------- ARENA INITIALIZE --------------------------------------------
	arena = GameObject(Arena);
	arena.loadTexture(TextureType::Diffuse, "textures/sand.png");
	arena.loadTexture(TextureType::Specular, "textures/fullSpecular.png");
	//arena.loadTexture(TextureType::Normal, "textures/normal.jpg"); //Normal Map Texture
	arena.rotate = glm::rotate(arena.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	arena.translate = glm::translate(arena.translate, glm::vec3(0.f, -1.f, 0.f));
	arena.scale = 1.8f;
	// -------------------------------------------- BARS INITIALIZE --------------------------------------------
	bars = GameObject(Bars);
	bars.loadTexture(TextureType::Diffuse, "textures/barsTextureFlipped.png");
	bars.loadTexture(TextureType::Specular, "textures/fullSpecular.png");
	bars.rotate = glm::rotate(bars.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	bars.translate = glm::translate(bars.translate, glm::vec3(0.f, -1.f, 0.f));
	bars.scale = 1.8f;
	// -------------------------------------------- BRICK INITIALZIE --------------------------------------------
	brick = GameObject(Brick);
	brick.loadTexture(TextureType::Diffuse, "textures/BrickTextureFinal.png");
	brick.loadTexture(TextureType::Specular, "textures/fullSpecular.png");
	brick.rotate = glm::rotate(brick.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	brick.translate = glm::translate(brick.translate, glm::vec3(0.f, -1.f, 0.f));
	brick.scale = 1.8f;
	// -------------------------------------------- SLAB INITIALZIE --------------------------------------------
	slab = GameObject(Slab);
	slab.loadTexture(TextureType::Diffuse, "textures/PodiumTextureV4.png");
	slab.loadTexture(TextureType::Specular, "textures/fullSpecular.png");
	slab.rotate = glm::rotate(slab.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	slab.translate = glm::translate(slab.translate, glm::vec3(0.f, -1.f, 0.f));
	slab.scale = 1.8f;
	// -------------------------------------------- PILLARS INITIALIZE --------------------------------------------
	pillars = GameObject(Pillars);
	pillars.loadTexture(TextureType::Diffuse, "textures/Pillar.png");
	pillars.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	//pillars.translate = glm::translate(pillars.translate, glm::vec3(0.f, 0.7f, 0.f));
	pillars.rotate = glm::rotate(pillars.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	pillars.scale = 1.5f;
	// -------------------------------------------- MONKEY HEAD LEFT INITIALIZE --------------------------------------------
	monkeyHeadLeft = GameObject(MonkeyHeadLeft);
	monkeyHeadLeft.loadTexture(TextureType::Diffuse, "textures/red.png");
	monkeyHeadLeft.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	monkeyHeadLeft.localRotation = 1.5708f;
	monkeyHeadLeft.rotate = glm::rotate(monkeyHeadLeft.rotate, monkeyHeadLeft.localRotation, glm::vec3(0.f, 1.f, 0.f));
	monkeyHeadLeft.scale = 1.8f;
	// -------------------------------------------- MONKEY HEAD RIGHT INITIALIZE --------------------------------------------
	monkeyHeadRight = GameObject(MonkeyHeadRight);
	monkeyHeadRight.loadTexture(TextureType::Diffuse, "textures/blue.png");
	monkeyHeadRight.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	monkeyHeadRight.localRotation = 1.5708f;
	monkeyHeadRight.rotate = glm::rotate(monkeyHeadRight.rotate, monkeyHeadRight.localRotation, glm::vec3(0.f, 1.f, 0.f));
	monkeyHeadRight.scale = 1.8f;
    // -------------------------------------------- PARTICLE BASE INITIALIZE --------------------------------------------
    emitterBase.mesh = partObj;
    emitterBase.loadTexture(TextureType::Specular, "textures/FullSpecular.png");
    emitterBase.loadTexture(TextureType::Diffuse, "textures/blue.png");
    emitterBase.loadTexture(TextureType::Specular, "textures/noSpecular.png");
    emitterBase.localRotation = 1.5708f;
    emitterBase.rotate = glm::rotate(knight1.rotate, emitterBase.localRotation, glm::vec3(0.f, 0.f, 1.f));
    emitterBase.scale = 0.1f;
	// -------------------------------------------- P1Wins INITIALIZE --------------------------------------------
	P1RoundWins.mesh = UIObj;
	P1RoundWins.loadTexture(TextureType::Diffuse, "textures/P1Wins.png");
	P1RoundWins.rotate = glm::rotate(P1RoundWins.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	P1RoundWins.translate = glm::translate(P1RoundWins.translate, glm::vec3(-9.f, 0.f, 8.f));
	P1RoundWins.scale = 0.5f;

	int numbOfWins = 5;
	for (int i = 0; i <= numbOfWins; i++)
	{
		UIP1Wins.push_back(new GameObject(P1RoundWins));

	}

	UIP1Wins[1]->loadTexture(TextureType::Diffuse, "textures/P1W1.png");
	UIP1Wins[2]->loadTexture(TextureType::Diffuse, "textures/P1W2.png");
	UIP1Wins[3]->loadTexture(TextureType::Diffuse, "textures/P1W3.png");
	UIP1Wins[4]->loadTexture(TextureType::Diffuse, "textures/P1W4.png");
	UIP1Wins[5]->loadTexture(TextureType::Diffuse, "textures/P1W5.png");


	// -------------------------------------------- P2Wins INITIALIZE --------------------------------------------
	P2RoundWins.mesh = UIObj;
	P2RoundWins.loadTexture(TextureType::Diffuse, "textures/P2Wins.png");
	//P2RoundWins.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	P2RoundWins.rotate = glm::rotate(P2RoundWins.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	P2RoundWins.translate = glm::translate(P2RoundWins.translate, glm::vec3(9.f, 0.f, 8.f));
	P2RoundWins.scale = 0.5f;


	for (int i = 0; i <= numbOfWins; i++)
	{
		UIP2Wins.push_back(new GameObject(P2RoundWins));
	}

	UIP2Wins[1]->loadTexture(TextureType::Diffuse, "textures/P2W1.png");
	UIP2Wins[2]->loadTexture(TextureType::Diffuse, "textures/P2W2.png");
	UIP2Wins[3]->loadTexture(TextureType::Diffuse, "textures/P2W3.png");
	UIP2Wins[4]->loadTexture(TextureType::Diffuse, "textures/P2W4.png");
	UIP2Wins[5]->loadTexture(TextureType::Diffuse, "textures/P2W5.png");

    //--------------------------------------------Skin Mesh Initialize----------------------------------------------------
    skinnedGameObject1.name = "root";

    skinnedGameObject1.mesh = SkinnedMesh;
    skinnedGameObject1.loadTexture(TextureType::Specular, "textures/FullSpecular.png");
    skinnedGameObject1.loadTexture(TextureType::Diffuse, "textures/red.png");
    skinnedGameObject1.loadTexture(TextureType::Specular, "textures/noSpecular.png");
    skinnedGameObject1.localRotation = 1.5708f;
    skinnedGameObject1.rotate = glm::rotate(knight1.rotate, emitterBase.localRotation, glm::vec3(0.f, 0.f, 1.f));

    skinnedGameObject1.initializeSkeletonFromHTR(animationPath + "jab.HTR", animationPath + "guy_weights.xml", skinnedGameObject1.mesh);
    skinnedGameObject1.setLocalScale(glm::vec3(1.0f));
    skinnedGameObject1.setScaleFloat(0.01f);


    std::cout << " NNN " << skinnedGameObject1.getScaleFloat() << std::endl;


    //skinnedGameObject1.transform = emitterBase.translate * emitterBase.rotate * glm::scale(glm::mat4(), glm::vec3(emitterBase.scale));
    skinnedGameObject1.transform = glm::translate(monkeyHeadLeft.transform, glm::vec3(-10.f,0.0f,0.0f) );



    // load emmiter set Magic.txt
    ReadEmitterFile("Magic.txt");

    // store emitters into game's emitter
    for (int i = 0; i < loadedEmitters.size(); i++) {

        emitters.push_back(*loadedEmitters[i]);
    }

    int itr = 0;
    emitterBase.transform = emitterBase.translate * emitterBase.rotate * glm::scale(glm::mat4(), glm::vec3(emitterBase.scale));

    for (int j = 0; j < emitters.size(); j++) {

        for (int i = 0; i < emitters[j].getNumParticles(); i++) {

            particleObj.push_back(new GameObject(emitterBase));
            //particleObj[itr]->mesh = partObj;
            // particleObj[i]->scale = 0.5f;
            // particleObj[i]->mesh.depth *= 0.5;
            // particleObj[i]->mesh.height *= 0.5;
            itr++;
        }
    }


	// -------------------------------------------- CAMERA INITIALIZE --------------------------------------------
	cameraTransform = glm::rotate(cameraTransform, 1.55f, glm::vec3(1.0f, 0.f, 0.f)) * glm::translate(cameraTransform, glm::vec3(0.f, -18.f, -2.f)); // camera position/rotation
	cameraProjection = glm::perspective(45.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10000.f); // camera perspective, 0.1 for near plane, 10000 for far plane
	cameraProjOrtho = glm::ortho(-15.0f, 15.0f, -8.0f, 7.0f, -100.0f, 100.0f);
}
void rotatePlayer(std::string objRot, int radian, glm::vec3 axis)
{

}
//Spline shit
int splineCounter = 0;
float timer = 0;
//Happens once per fram, used to update state of the game
void Game::update()
{

	updateTime->tick();
	float deltaTime = updateTime->getElapsedTimeSeconds();

	if (doDeCast)
	{
		//update timer so we have correct delta time since last update	
		timer += deltaTime;
		if (timer > 1) {

			if (splineCounter <= 0) //0 will play once
									//2 will play 3 times etc
				splineTime += deltaTime*0.2; //update spline time, multiple the dt by a fraction to slow down

			//Math function call
			glm::vec3 camPos(Math::deCastelya(glm::vec3(0.f, -18.f, -2.f), glm::vec3(30.f, -10.f, 8.f), glm::vec3(-30.f, -20.f, 8.f), glm::vec3(6.f, -20.f, -2.f), splineTime));
			//Pack into camera transform function
			cameraTransform = glm::rotate(glm::mat4(1.0f), 1.5f, glm::vec3(1.0f, 0.f, 0.f)) * glm::translate(glm::mat4(1.0f), camPos); // camera position/rotation

			if (splineTime >= 1.f)
			{
				splineTime = 0.f;
				splineCounter++;
			}
		}
	}

    //Skin Mesh Update
    // Update the GameObjects as necessary
    skinnedGameObject1.deltaTime += deltaTime;
    if (skinnedGameObject1.m_pPlaying)
        skinnedGameObject1.update(deltaTime);

	// Animation Update
	float speed = 5.f;
	knight1Animation.animation.t += deltaTime*speed;
	if (knight1Animation.animation.t > 1.f)
	{
		knight1Animation.animation.currentFrame++;

		if (knight1Animation.animation.keyFrames.size() <= knight1Animation.animation.currentFrame)
		{
			knight1Animation.animation.currentFrame = 0;
		}
		knight1Animation.animation.t = 0;
	}
	knight1Animation.mesh = (knight1Animation.animation.keyFrames[knight1Animation.animation.currentFrame]);

    // Particle Movement Update
    for (int i = 0; i < emitters.size(); i++) {
        emitters[i].update(deltaTime);
    }


    int itr = 0;


	// -------------------------------------------- Controller Input --------------------------------------------
	Input::XBoxInput XBoxController;
	XBoxController.DownloadPackets(2); // the first parameter is the number of controllers connected
	if (XBoxController.GetConnected(0))
	{
		// -------------------------------------------- LEFT THUMB STICK PLAYER 1 --------------------------------------------
		Input::Stick lStick1, rStick1;
		XBoxController.GetSticks(0, lStick1, rStick1);

		knight1.Movement(lStick1);

		knight1Animation.translate = knight1.translate;
		knight1Animation.rotate = knight1.rotate;

		P1.Shield.translate = knight1.translate;
		P1.Shield.rotate = glm::rotate(knight1.rotate, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
		
		// -------------------------------------------- RIGHT THUMB STICK PLAYER 1 --------------------------------------------
		// Get Opposite Angle Player 1 is facing (for shield hit detection)
		if ( ((glm::atan(rStick1.yAxis, rStick1.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) >= 0 )
			knight1.OppAngle = ((glm::atan(rStick1.yAxis, rStick1.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) - glm::pi<float>();
		else if ( ((glm::atan(rStick1.yAxis, rStick1.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) < 0 )
			knight1.OppAngle = ((glm::atan(rStick1.yAxis, rStick1.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) + glm::pi<float>();

		// Dead Zone Check
		if (sqrt(rStick1.yAxis*rStick1.yAxis + rStick1.xAxis*rStick1.xAxis) > 0.1f) // DeadZone 10%
		{
			knight1.rotate = glm::rotate(glm::mat4(), ((glm::atan(rStick1.yAxis, rStick1.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) + (float(1.5708)), glm::vec3(0.f, 1.f, 0.f));
			knight1.localRotation = ((glm::atan(rStick1.yAxis, rStick1.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) + (-float(1.5708)); // Update local rotation for bullet

			P1.Shield.position.x = knight1.position.x + glm::cos(knight1.OppAngle) * 0.4f;
			P1.Shield.position.z = knight1.position.z + glm::sin(knight1.OppAngle) * 0.4f;
		}
		

		// -------------------------------------------- RIGHT TRIGGER PLAYER 1 --------------------------------------------
		float lTrig1, rTrig1;
		XBoxController.GetTriggers(0, lTrig1, rTrig1);
		p1Bullets[P1.BulletNum] = new GameObject(Bullet); // This remakes the bullet GameObject and destroys the old one whenever RT is pressed
		if (rTrig1 == 1)
		{
			P1.Shoot(&knight1, p1Bullets, rStick1); // -------------------------------------------- SHOOT BULLET --------------------------------------------

			if (P1.once == true && P1.CD == false) // If once1 is true, bulletNum changes to the next num in line
			{
				if (P1.BulletNum < 4)
					P1.BulletNum += 1;
				else // If past the max bullet pool, player will take control of the first bullet
					P1.BulletNum = 0;
				P1.once = false;
				P1.CD = true;
				System->playSound(sound2, nullptr, false, &channel2);
			}
		}
		if (P1.CD) // Bullet Cool Down
		{
			P1.BulletCD += deltaTime;
			if (P1.BulletCD > 1)
			{
				P1.BulletCD = 0;
				P1.CD = false;
			}
		}
		if (rTrig1 == 0) // If RT is not pressed the bulletNum changes once
			P1.once = true;

		if (sqrt(rStick1.yAxis*rStick1.yAxis + rStick1.xAxis*rStick1.xAxis) > 0.1f) // If magnitude of rStick is greater than 10%, update prevStick
		{
			P1.PrevStickX = rStick1.xAxis;
			P1.PrevStickY = rStick1.yAxis;
		}

		

		// -------------------------------------------- Bullet Pillar Collision --------------------------------------------
		// A Shit Ton of Bullshit Hardcoded Pillar Collision w/ Bullets is below.
		{
			// Bullet 1 and Top Pillar Collision for p1

			for (int i = 0; i < p1Bullets.size(); i++) // change p1Bullets into function call variable
			{
				if (1.6 > p1Bullets[i]->position.x + P1.BulletMoveVal[i].x && p1Bullets[i]->position.x + P1.BulletMoveVal[i].x > -1.6 && -2.9 > p1Bullets[i]->position.z + P1.BulletMoveVal[i].y && p1Bullets[i]->position.z + P1.BulletMoveVal[i].y > -5.9)
				{
					if (1.6 < p1Bullets[i]->position.x - P1.BulletMoveVal[i].x || p1Bullets[i]->position.x - P1.BulletMoveVal[i].x < -1.6)
						P1.BulletMoveVal[i].x *= -1, P1.BulletBounceNum[i] += 0.5;
					else if (-2.9 < p1Bullets[i]->position.z - P1.BulletMoveVal[i].y || p1Bullets[i]->position.z - P1.BulletMoveVal[i].y < -5.9)
						P1.BulletMoveVal[i].y *= -1, P1.BulletBounceNum[i] += 0.5;
				}
				//Bullet 1 and Middle Pillar Collision for p1
				if (1.6 > p1Bullets[i]->position.x + P1.BulletMoveVal[i].x && p1Bullets[ i ]->position.x + P1.BulletMoveVal[i].x > -1.6 && 1.4 > p1Bullets[i]->position.z + P1.BulletMoveVal[i].y && p1Bullets[i]->position.z + P1.BulletMoveVal[i].y > -1.4)
				{
					if (1.6 < p1Bullets[i]->position.x - P1.BulletMoveVal[i].x || p1Bullets[i]->position.x - P1.BulletMoveVal[i].x < -1.6)
						P1.BulletMoveVal[i].x *= -1, P1.BulletBounceNum[i] += 0.5;
					else if (1.4 < p1Bullets[i]->position.z - P1.BulletMoveVal[i].y || p1Bullets[i]->position.z - P1.BulletMoveVal[i].y < -1.4)
						P1.BulletMoveVal[i].y *= -1, P1.BulletBounceNum[i] += 0.5;
				}
				//Bullet 1 and Bottom Pillar Collision for p1
				if (1.6 > p1Bullets[i]->position.x + P1.BulletMoveVal[i].x && p1Bullets[i]->position.x + P1.BulletMoveVal[i].x > -1.6 && 5.9 > p1Bullets[i]->position.z + P1.BulletMoveVal[i].y && p1Bullets[i]->position.z + P1.BulletMoveVal[i].y > 2.9)
				{
					if (1.6 < p1Bullets[i]->position.x - P1.BulletMoveVal[i].x || p1Bullets[i]->position.x - P1.BulletMoveVal[i].x < -1.6)
						P1.BulletMoveVal[i].x *= -1, P1.BulletBounceNum[i] += 0.5;
					else if (5.9 < p1Bullets[i]->position.z - P1.BulletMoveVal[i].y || p1Bullets[i]->position.z - P1.BulletMoveVal[i].y < 2.9)
						P1.BulletMoveVal[i].y *= -1, P1.BulletBounceNum[i] += 0.5;
				}

			}
			
			
		}

		//-------------------------------------------- Outer Walls Bullet Collision Check --- PLAYER 1 --------------------------------------------

		for (int i = 0; i < p1Bullets.size(); i++) // change p1Bullets into function call variable
		{
			if (p1Bullets[i]->position.x < -13.5 || p1Bullets[i]->position.x > 13.2)
				P1.BulletMoveVal[i].x *= -1, P1.BulletBounceNum[i]++;
			if (p1Bullets[i]->position.z < -7.35 || p1Bullets[i]->position.z > 7.55)
				P1.BulletMoveVal[i].y *= -1, P1.BulletBounceNum[i]++;
		}
		// Bullet Bounce Num Check (Bullets are Destroyed after 3 bounces) 

		for (int i = 0; i < p1Bullets.size(); i++) // change p1Bullets into function call variable
		{
			if (P1.BulletBounceNum[i] > 3)
			{
				P1.BulletMoveVal[i].x = 0, P1.BulletMoveVal[i].y = 0;
				p1Bullets[i]->position.x = 0;
				p1Bullets[i]->position.z = 0;
				p1Bullets[i]->translate = glm::translate(glm::mat4(), p1Bullets[i]->position); //set position of object
				P1.BulletBounceNum[i] = 0;
			}
		}
		
	}
	if (XBoxController.GetConnected(1)) {
		// -------------------------------------------- LEFT THUMB STICK PLAYER 2 --------------------------------------------
		Input::Stick lStick2, rStick2;
		XBoxController.GetSticks(1, lStick2, rStick2);

		knight2.Movement(lStick2);

		P2.Shield.translate = knight2.translate;
		P2.Shield.rotate = glm::rotate(knight2.rotate, 0.f, glm::vec3(0.0f, 1.0f, 0.0f)); // Does not do anything

		// -------------------------------------------- RIGHT THUMB STICK PLAYER 2 --------------------------------------------
		// Get Opposite angle Player 2 is facing
		if (((glm::atan(rStick2.yAxis, rStick2.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) >= 0)
			knight2.OppAngle = ((glm::atan(rStick2.yAxis, rStick2.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) - glm::pi<float>();
		else if (((glm::atan(rStick2.yAxis, rStick2.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) < 0)
			knight2.OppAngle = ((glm::atan(rStick2.yAxis, rStick2.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) + glm::pi<float>();

		// Dead Zone Check
		if (sqrt(rStick2.yAxis*rStick2.yAxis + rStick2.xAxis*rStick2.xAxis) > 0.1f) // DeadZone 10%
		{
			knight2.rotate = glm::rotate(glm::mat4(), ((glm::atan(rStick2.yAxis, rStick2.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) + (-float(1.5708)), glm::vec3(0.f, 1.f, 0.f));
			knight2.localRotation = ((glm::atan(rStick2.yAxis, rStick2.xAxis)) * (180.f / glm::pi<float>()) * float(0.0174533)) + (-float(1.5708)); // Update local rotation for bullet
		
			P2.Shield.position.x = knight2.position.x + glm::cos(knight2.OppAngle) * 0.4f;
			P2.Shield.position.z = knight2.position.z + glm::sin(knight2.OppAngle) * 0.4f;
		}
		// -------------------------------------------- RIGHT TRIGGER PLAYER 2 --------------------------------------------
		float lTrig2, rTrig2;
		XBoxController.GetTriggers(1, lTrig2, rTrig2);
		p2Bullets[P2.BulletNum] = new GameObject(Bullet); // This remakes the bullet GameObject and destroys the old one whenever RT is pressed
		if (rTrig2 == 1)
		{
			P2.Shoot(&knight2, p2Bullets, rStick2);

			if (P2.once == true && P2.CD == false) // If once1 is true, bulletNum changes to the next num in line
			{
				if (P2.BulletNum < 4)
					P2.BulletNum += 1;
				else // If past the max bullet pool, player will take control of the first bullet
					P2.BulletNum = 0;
				P2.once = false;
				P2.CD = true;
				System->playSound(sound2, nullptr, false, &channel2);
			}
		}
		if (P2.CD) // Bullet Cool Down
		{
			P2.BulletCD += deltaTime;
			if (P2.BulletCD > 1)
			{
				P2.BulletCD = 0;
				P2.CD = false;
			}
		}
		if (rTrig2 == 0) // If RT is not pressed the bulletNum changes once
			P2.once = true;

		if (sqrt(rStick2.yAxis*rStick2.yAxis + rStick2.xAxis*rStick2.xAxis) > 0.1f) // If magnitude of rStick is greater than 10%, update prevStick
		{
			P2.PrevStickX = rStick2.xAxis;
			P2.PrevStickY = rStick2.yAxis;
		}


		// -------------------------------------------- BULLET PILLAR COLLISIONS --------------------------------------------
		{
			// Player 2
			for (int i = 0; i < p2Bullets.size(); i++) // change p2Bullets into function call variable
			{
				if (1.6 > p2Bullets[i]->position.x + P2.BulletMoveVal[i].x && p2Bullets[i]->position.x + P2.BulletMoveVal[i].x > -1.6 && -2.9 > p2Bullets[i]->position.z + P2.BulletMoveVal[i].y && p2Bullets[i]->position.z + P2.BulletMoveVal[i].y > -5.9)
				{
					if (1.6 < p2Bullets[i]->position.x - P2.BulletMoveVal[i].x || p2Bullets[i]->position.x - P2.BulletMoveVal[i].x < -1.6)
						P2.BulletMoveVal[i].x *= -1, P2.BulletBounceNum[i] += 0.5;
					else if (-2.9 < p2Bullets[i]->position.z - P2.BulletMoveVal[i].y || p2Bullets[i]->position.z - P2.BulletMoveVal[i].y < -5.9)
						P2.BulletMoveVal[i].y *= -1, P2.BulletBounceNum[i] += 0.5;
				}
				//Bullet 1 and Middle Pillar Collision for p2
				if (1.6 > p2Bullets[i]->position.x + P2.BulletMoveVal[i].x && p2Bullets[i]->position.x + P2.BulletMoveVal[i].x > -1.6 && 1.4 > p2Bullets[i]->position.z + P2.BulletMoveVal[i].y && p2Bullets[i]->position.z + P2.BulletMoveVal[i].y > -1.4)
				{
					if (1.6 < p2Bullets[i]->position.x - P2.BulletMoveVal[i].x || p2Bullets[i]->position.x - P2.BulletMoveVal[i].x < -1.6)
						P2.BulletMoveVal[i].x *= -1, P2.BulletBounceNum[i] += 0.5;
					else if (1.4 < p2Bullets[i]->position.z - P2.BulletMoveVal[i].y || p2Bullets[i]->position.z - P2.BulletMoveVal[i].y < -1.4)
						P2.BulletMoveVal[i].y *= -1, P2.BulletBounceNum[i] += 0.5;
				}
				//Bullet 1 and Bottom Pillar Collision for p2
				if (1.6 > p2Bullets[i]->position.x + P2.BulletMoveVal[i].x && p2Bullets[i]->position.x + P2.BulletMoveVal[i].x > -1.6 && 5.9 > p2Bullets[i]->position.z + P2.BulletMoveVal[i].y && p2Bullets[i]->position.z + P2.BulletMoveVal[i].y > 2.9)
				{
					if (1.6 < p2Bullets[i]->position.x - P2.BulletMoveVal[i].x || p2Bullets[i]->position.x - P2.BulletMoveVal[i].x < -1.6)
						P2.BulletMoveVal[i].x *= -1, P2.BulletBounceNum[i] += 0.5;
					else if (5.9 < p2Bullets[i]->position.z - P2.BulletMoveVal[i].y || p2Bullets[i]->position.z - P2.BulletMoveVal[i].y < 2.9)
						P2.BulletMoveVal[i].y *= -1, P2.BulletBounceNum[i] += 0.5;
				}

			}
			// -------------------------------------------- Outer Walls/Pillar Bullet Collision Check --- PLAYER 2  --------------------------------------------

			// Player 2
			for (int i = 0; i < p2Bullets.size(); i++) // change p2Bullets into function call variable
			{
				if (p2Bullets[i]->position.x < -13.5 || p2Bullets[i]->position.x > 13.2)
					P2.BulletMoveVal[i].x *= -1, P2.BulletBounceNum[i]++;
				if (p2Bullets[i]->position.z < -7.35 || p2Bullets[i]->position.z > 7.55)
					P2.BulletMoveVal[i].y *= -1, P2.BulletBounceNum[i]++;
			}

			// Bullet Bounce Num Check (Bullets are Destroyed after 3 bounces) 

			// Player 2
			for (int i = 0; i < p2Bullets.size(); i++) // change p2Bullets into function call variable
			{
				if (P2.BulletBounceNum[i] > 3)
				{
					P2.BulletMoveVal[i].x = 0, P2.BulletMoveVal[i].y = 0;
					p2Bullets[i]->position.x = 0;
					p2Bullets[i]->position.z = 0;
					p2Bullets[i]->translate = glm::translate(glm::mat4(), p2Bullets[i]->position); //set position of object
					P2.BulletBounceNum[i] = 0;
				}
			}
		}
	}




	// -------------------------------------------- Shield Collision Player 1 --------------------------------------------
	for (int i = 0; i <= 4; i++)
	{
		if (glm::distance(p2Bullets[i]->position, P1.Shield.position) < P1.Shield.Radius)
		{
			std::cout << "P1Shield was Hit" << std::endl;
			P2.BulletMoveVal[i].x = glm::reflect(P2.BulletMoveVal[i], glm::vec2(glm::cos(knight1.OppAngle), glm::sin(knight1.OppAngle))).x;
			P2.BulletMoveVal[i].y = glm::reflect(P2.BulletMoveVal[i], glm::vec2(glm::cos(knight1.OppAngle), glm::sin(knight1.OppAngle))).y;
			P2.BulletBounceNum[i]++;
		}
	}

	// -------------------------------------------- Shield Collision Player 2 --------------------------------------------
	for (int i = 0; i <= 4; i++)
	{
		if (glm::distance(p1Bullets[i]->position, P2.Shield.position) < P2.Shield.Radius)
		{
			std::cout << "P2Shield was Hit" << std::endl;
			P1.BulletMoveVal[i].x = glm::reflect(P1.BulletMoveVal[i], glm::vec2(glm::cos(knight2.OppAngle), glm::sin(knight2.OppAngle))).x;
			P1.BulletMoveVal[i].y = glm::reflect(P1.BulletMoveVal[i], glm::vec2(glm::cos(knight2.OppAngle), glm::sin(knight2.OppAngle))).y;
			P1.BulletBounceNum[i]++;
		}
	}

	// -------------------------------------------- Player 1 Death --------------------------------------------
	if (glm::distance(p2Bullets[0]->position, knight1.position) < knight1.Radius) // CIRCLE COLLISION CHECK
	{
		P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);
	}
	else if (glm::distance(p2Bullets[1]->position, knight1.position) < knight1.Radius)
	{
		P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);
	}
	else if (glm::distance(p2Bullets[2]->position, knight1.position) < knight1.Radius)
	{
		P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);
	}
	else if (glm::distance(p2Bullets[3]->position, knight1.position) < knight1.Radius)
	{
		P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);
	}
	else if (glm::distance(p2Bullets[4]->position, knight1.position) < knight1.Radius)
	{
		P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);
	}
	// -------------------------------------------- Player 2 Death --------------------------------------------
	if (glm::distance(p1Bullets[0]->position, knight2.position) < knight2.Radius) // CIRCLE COLLISION CHECK
	{
		P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
	}
	else if (glm::distance(p1Bullets[1]->position, knight2.position) < knight2.Radius)
	{
		P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
	}
	else if (glm::distance(p1Bullets[2]->position, knight2.position) < knight2.Radius)
	{
		P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
	}
	else if (glm::distance(p1Bullets[3]->position, knight2.position) < knight2.Radius)
	{
		P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
	}
	else if (glm::distance(p1Bullets[4]->position, knight2.position) < knight2.Radius)
	{
		P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
	}

	// -------------------------------------------- Translate if bullets are supposed to move --------------------------------------------
	if (P1.BulletMove == true)
	{
		for (int i = 0; i < p1Bullets.size(); i++)
		{
			p1Bullets[i]->translate = glm::translate(p1Bullets[i]->translate, glm::vec3(P1.BulletMoveVal[i].x, 0.f, P1.BulletMoveVal[i].y));
		}
	}

	if (P2.BulletMove == true)
	{
		for (int i = 0; i < p1Bullets.size(); i++)
		{
			p2Bullets[i]->translate = glm::translate(p2Bullets[i]->translate, glm::vec3(P2.BulletMoveVal[i].x, 0.f, P2.BulletMoveVal[i].y));
		}
	}
	if (doParenting == true)
	{
		cameraTransform = glm::rotate(glm::mat4(1.0f), 1.5f, glm::vec3(1.0f, 0.f, 0.f)) * glm::translate(glm::mat4(1.0f), glm::vec3(knight1.position.x * -1, -18.f, -2.f));
	}

	// ---------------------- F = T * R * S ----------------------
	knight1Animation.transform = knight1Animation.translate * knight1Animation.rotate * glm::scale(glm::mat4(), glm::vec3(knight1Animation.scale));
	P1.Shield.transform = P1.Shield.translate * P1.Shield.rotate * glm::scale(glm::mat4(), glm::vec3(P1.Shield.scale));
	P2.Shield.transform = P2.Shield.translate * P2.Shield.rotate * glm::scale(glm::mat4(), glm::vec3(P2.Shield.scale));

	knight1.transform = knight1.translate * knight1.rotate * glm::scale(glm::mat4(), glm::vec3(knight1.scale));
	knight2.transform = knight2.translate * knight2.rotate * glm::scale(glm::mat4(), glm::vec3(knight2.scale));
	arena.transform = arena.translate * arena.rotate * glm::scale(glm::mat4(), glm::vec3(arena.scale));
	bars.transform = bars.translate * bars.rotate * glm::scale(glm::mat4(), glm::vec3(bars.scale));
	brick.transform = brick.translate * brick.rotate * glm::scale(glm::mat4(), glm::vec3(brick.scale));
	slab.transform = slab.translate * slab.rotate * glm::scale(glm::mat4(), glm::vec3(brick.scale));
	pillars.transform = pillars.translate * pillars.rotate * glm::scale(glm::mat4(), glm::vec3(pillars.scale));
	monkeyHeadLeft.transform = monkeyHeadLeft.translate * monkeyHeadLeft.rotate * glm::scale(glm::mat4(), glm::vec3(monkeyHeadLeft.scale));
	monkeyHeadRight.transform = monkeyHeadRight.translate * monkeyHeadRight.rotate * glm::scale(glm::mat4(), glm::vec3(monkeyHeadRight.scale));

    skinnedGameObject1.drawMatrix();
    //Skinned mesh Transform
   // skinnedGameObject1.transform = glm::translate(glm::mat4(), glm::vec3(1.0f));

	P1RoundWins.transform = P1RoundWins.translate * P1RoundWins.rotate * glm::scale(glm::mat4(), glm::vec3(P1RoundWins.scale));
	P2RoundWins.transform = P2RoundWins.translate * P2RoundWins.rotate * glm::scale(glm::mat4(), glm::vec3(P2RoundWins.scale));
	if (P1.RoundWins < UIP1Wins.size())
	{  
		UIP1Wins[P1.RoundWins]->transform = P1RoundWins.transform;
	}
	else
	{
		UIP1Wins.back()->transform = P1RoundWins.transform;
	}
	if (P2.RoundWins < UIP2Wins.size())
	{
		UIP2Wins[P2.RoundWins]->transform = P2RoundWins.transform;
	}
	else
	{
		UIP2Wins.back()->transform = P2RoundWins.transform;
	}

	knight1.position = glm::vec3(knight1.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	knight2.position = glm::vec3(knight2.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	for (int i = 0; i < p1Bullets.size(); i++)
	{
		p1Bullets[i]->transform = p1Bullets[i]->translate * p1Bullets[i]->rotate * glm::scale(glm::mat4(), glm::vec3(p1Bullets[i]->scale));
		p2Bullets[i]->transform = p2Bullets[i]->translate * p2Bullets[i]->rotate * glm::scale(glm::mat4(), glm::vec3(p2Bullets[i]->scale));

		p1Bullets[i]->position = glm::vec3(p1Bullets[i]->transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		p2Bullets[i]->position = glm::vec3(p2Bullets[i]->transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

    for (int j = 0; j < emitters.size(); j++) {
        for (int i = 0; i < emitters[j].getNumParticles(); i++) {

            //    for (int i = 0; i < particleObj.size(); i++) {
            particleObj[itr]->transform = glm::translate(knight1.transform, emitters[j].getParticlePosition(i) / 50.0f + glm::vec3(-5.0, 3.f, 0.0f))  * particleObj[itr]->scale;
            itr++;
        }
    }

	if (shouldLightsSpin)
	{
		lightSpinner = glm::rotate(lightSpinner, deltaTime *(glm::pi<float>() / 2.f), glm::vec3(0.f, 0.f, 1.f));
		for (int i = 0; i < pointLights.size(); i++)
		{
			pointLights[i].positionOrDirection = lightSpinner * pointLights[i].originalPosition;
		}
	}
}
void Game::draw()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// -------------------------------------------- DRAW CALLS --------------------------------------------
	knight2.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	
	for (int i = 0; i < p1Bullets.size(); i++)
	{
		if (P1.BulletBounceNum[i] < 3)
			p1Bullets[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
		if (P2.BulletBounceNum[i] < 3)
			p2Bullets[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
		
	}
	arena.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	bars.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	brick.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	slab.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	pillars.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	knight1Animation.draw(animationShader, cameraTransform, cameraProjection, pointLights, directionalLight); //knight animation 1 walk
	P1.Shield.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	P2.Shield.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	// MONKEY HEAD WIN CONDITION
	if (P1.RoundWins >= 5)
		monkeyHeadLeft.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	if (P2.RoundWins >= 5)
		monkeyHeadRight.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	if (P1.RoundWins < UIP1Wins.size())
	{
		UIP1Wins[P1.RoundWins]->draw(UI, cameraTransform, cameraProjOrtho, pointLights, directionalLight);
	}
	else
	{
		UIP1Wins.back()->draw(UI, cameraTransform, cameraProjOrtho, pointLights, directionalLight);
	}

	if( P2.RoundWins < UIP2Wins.size() )
	{
		UIP2Wins[P2.RoundWins]->draw(UI, cameraTransform, cameraProjOrtho, pointLights, directionalLight);
	}
	else
	{
		UIP2Wins.back()->draw(UI, cameraTransform, cameraProjOrtho, pointLights, directionalLight);
	}

    skinnedGameObject1.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);


    {
        //switch (P1.RoundWins)
        //{
        //case 1:
        //	for (static bool once = true; once; once = false) // THIS RUNS CODE ONCE IN A LOOP
        //		(P1RoundWins.loadTexture(TextureType::Diffuse, "textures/P1W1.png"));
        //	break;
        //case 2:
        //	for (static bool once = true; once; once = false)
        //		P1RoundWins.loadTexture(TextureType::Diffuse, "textures/P1W2.png");
        //	break;
        //case 3:
        //	for (static bool once = true; once; once = false)
        //		P1RoundWins.loadTexture(TextureType::Diffuse, "textures/P1W3.png");
        //	break;
        //case 4:
        //	for (static bool once = true; once; once = false)
        //		P1RoundWins.loadTexture(TextureType::Diffuse, "textures/P1W4.png");
        //	break;
        //case 5:
        //	for (static bool once = true; once; once = false)
        //		P1RoundWins.loadTexture(TextureType::Diffuse, "textures/P1W5.png");
        //	break;
        //default:
        //	break;
        //}
        //switch (P2.RoundWins)
        //{
        //case 1:
        //	for (static bool once = true; once; once = false)
        //		P2RoundWins.loadTexture(TextureType::Diffuse, "textures/P2W1.png");
        //	break;
        //case 2:
        //	for (static bool once = true; once; once = false)
        //		P2RoundWins.loadTexture(TextureType::Diffuse, "textures/P2W2.png");
        //	break;
        //case 3:
        //	for (static bool once = true; once; once = false)
        //		P2RoundWins.loadTexture(TextureType::Diffuse, "textures/P2W3.png");
        //	break;
        //case 4:
        //	for (static bool once = true; once; once = false)
        //		P2RoundWins.loadTexture(TextureType::Diffuse, "textures/P2W4.png");
        //	break;
        //case 5:
        //	for (static bool once = true; once; once = false)
        //		P2RoundWins.loadTexture(TextureType::Diffuse, "textures/P2W5.png");
        //	break;
        //default:
        //	break;
        //}
    }

    int itr = 0;
    glm::vec3 ambientHold = directionalLight.ambient;
    glm::vec3 diffuseHold = directionalLight.diffuse;
    glm::vec3 specularHold = directionalLight.specular;

    for (int j = 0; j < emitters.size(); j++) {

        for (int i = 0; i < emitters[j].getNumParticles(); i++) {

            //  for (int i = 0; i < particleObj.size(); i++) {
            if (emitters[j].getParticleLife(i) > 0) {
                directionalLight.ambient = emitters[j].getParticleColor(i);
                directionalLight.diffuse = emitters[j].getParticleColor(i) * 0.5f;
                directionalLight.specular = emitters[j].getParticleColor(i) * 0.5f;

                particleObj[itr]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
                itr++;
            }
        }
    }
    directionalLight.ambient = ambientHold;
    directionalLight.diffuse = diffuseHold;
    directionalLight.specular = specularHold;

	glutSwapBuffers();
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
	case 27: //esc key	
	case 'q':
		exit(1);
		break;
	case 32:
		System->playSound(sound2, nullptr, false, &channel2);
		break;
	case 't':
		std::cout << "Total elapsed time: " << updateTime->getCurrentTime() / 1000.f << std::endl;
		break;
	case 'w':
		wKeydown = true;
		break;
	case 'a':
		//Plays shot sound
		aKeydown = true;
		break;
	case 's':
		sKeydown = true;
		break;
	case 'd':
		dKeydown = true;
		break;
	case 'o':
		oKeydown = true;
		break;
	case 'p':
		pKeydown = true;
		break;
	case 'i':
		iKeyDown = true;
		break;
	case 'k':
		kKeyDown = true;
		break;
	case 'u':
		uKeyDown = true;
		break;
	case 'j':
		jKeyDown = true;
		break;
	default:
		break;
	}
}
void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
	case 'r':
		shouldRotate = !shouldRotate;
		break;
	case 'l':
		shouldLightsSpin = !shouldLightsSpin;
		break;
	case 'w':
		wKeydown = false;
		P1.RoundWins++; // ----------------------------------------------------------- DEBUGGING (DELETE THIS) -------------------------------------------------------------
		break;
	case 'a':
		aKeydown = false;
		break;
	case 's':
		sKeydown = false;
		P2.RoundWins++; // ----------------------------------------------------------- DEBUGGING (DELETE THIS) -------------------------------------------------------------
		break;
	case 'd':
        dKeydown = false;
		doDeCast = !doDeCast;
		break;
	case 'o':
		oKeydown = false;
		break;
	case 'p':
		doParenting = !doParenting;
		break;
	case 'i':
		iKeyDown = false;
		break;
	case 'k':
		kKeyDown = false;
		break;
	case 'u':
		uKeyDown = false;
		break;
	case 'j':
		jKeyDown = false;
		break;
    case '+':
      //  skinnedGameObject1.setScaleFloat(skinnedGameObject1.getScaleFloat() + 0.05f);
        skinnedGameObject1.transform = glm::translate(skinnedGameObject1.transform, glm::vec3(-2.0f, 0.0f, 0.0f));
        break;
    case '-':
       // skinnedGameObject1.setScaleFloat(skinnedGameObject1.getScaleFloat() - 0.05f);
        skinnedGameObject1.transform = glm::translate(skinnedGameObject1.transform, glm::vec3(2.0f, 0.0f, 0.0f));
        break;
	default:
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y) 
{
	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			//handle left click
			break;
		case GLUT_RIGHT_BUTTON:
			//handle right click
			break;
		case GLUT_MIDDLE_BUTTON:
			//handle middle mouse
			break;
		default:
			break;
		}
	}
}
void Game::mouseMoved(int x, int y)
{

}
void Game::ReadEmitterFile(std::string fileName)
{
    std::string line;
    std::string name = fileName;

    std::ifstream myfile(name);
    if (myfile.is_open())
    {
        std::getline(myfile, line);

        float size2 = std::strtof(line.c_str(), 0);
        std::cout << size2 << std::endl;

        std::getline(myfile, line);
        if (line == ":")
            std::cout << line << std::endl;

        for (int i = 0; i <= size2; i++) {


            loadedEmitters.push_back(new ParticleEmitter());
            float size = 0;

            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->playing = size;

            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->interpolateColour = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->velocity0.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->velocity0.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->velocity0.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->velocity1.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->velocity1.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->velocity1.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->emitterPosition.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->emitterPosition.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->emitterPosition.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->boxWH.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->boxWH.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->boxWH.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->boxOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->taretgOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->fleeOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->magnetOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->repelOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->gravityOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->pathOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->targetPos.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->targetPos.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->targetPos.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->fleePos.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->fleePos.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->fleePos.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->magnetPos.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->magnetPos.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->magnetPos.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->repelPos.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->repelPos.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->repelPos.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->gravity.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->gravity.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->gravity.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->pathEnd.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->pathEnd.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->pathEnd.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->targetStrength = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->fleeStrength = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->magnetStrength = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->magnetRadius = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->repelStrength = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->repelRadius = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->colour0.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->colour0.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->colour0.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->colour1.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->colour1.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->colour1.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->lifeRange.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->lifeRange.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->sizeRange.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->sizeRange.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->spawnRange.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->spawnRange.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->massRange.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->massRange.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->durationOn = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->durationRepeat = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->durationOnX_OffY.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->durationOnX_OffY.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->keyframeController.curvePlaySpeed = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->catmullT0.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->catmullT0.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->catmullT0.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->catmullT1.x = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->catmullT1.y = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->catmullT1.z = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);

            loadedEmitters[loadedEmitters.size() - 1]->pathPointStrength = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->step = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->showEmitter = size;
            std::getline(myfile, line);
            size = std::strtof(line.c_str(), 0);
            loadedEmitters[loadedEmitters.size() - 1]->circleSquare = size;

            std::getline(myfile, line);
            if (line == ":")
                std::cout << line << std::endl;
            std::getline(myfile, line);
            if (line == "*")
                std::cout << line << std::endl;

            loadedEmitters[loadedEmitters.size() - 1]->initialize(50);

        }
        myfile.close();

    }

    else std::cout << "Unable to open file";


}
