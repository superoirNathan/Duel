#include "Game.h"
#include "controller.h"
#include <iostream>
#include <fstream>  // added
#include <Windows.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FMOD\fmod.hpp>
#include "animationMath.h"
#include "SkinnedGameObject.h"

Game::Game()
{

    //Initialize FMOD
    se.Init();
}
Game::~Game()
{
    delete updateTime;

    passThroughParticles.unload();
}
float splineTime = 0;
GameObject shaderNumber;

//happens once at the beginning of the game
void Game::initializeGame()
{
	shaderNumber.shaderNumber = 1;
    updateTime = new Timer();

    // TIMER FOR LOADING
    updateTime->tick();
    float deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer

    glEnable(GL_DEPTH_TEST);

    ////Create the Sounds *Doesn't play the sound*
    //System->createSound("Sounds/POKEMANZ.mp3", FMOD_DEFAULT, 0, &sound1);
    //System->createSound("Sounds/Sound_Shot.mp3", FMOD_DEFAULT, 0, &sound2);
    //System->createSound("Sounds/deflect.wav", FMOD_DEFAULT, 0, &sound3);

    ////Set the mode of the sound, not nessasary
    //sound1->setMode(FMOD_LOOP_NORMAL); //No Loop
    //sound2->setMode(FMOD_LOOP_OFF); //Loops
    //sound3->setMode(FMOD_LOOP_OFF); //Loops
    //                                //Update *System for sounds
    //System->update();


    //lights
    Light light1;//, light2;
	light1.positionOrDirection = glm::vec4(103.f, 31.f, 132.f, 1.f);
	light1.originalPosition = glm::vec4(5.f, 6.f, -7.f, 1.f);
    light1.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    light1.diffuse = glm::vec3(255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
    light1.specular = glm::vec3(1.f, 1.f, 1.f);
    light1.specularExponent = 45.f;
    light1.constantAttenuation = 1.f;
    light1.linearAttenuation = 0.1f;
    light1.quadraticAttenuation = 0.022f;
	

    //Only 1 Point Light
    pointLight = light1;

    //Load Shaders
    phong.load("shaders/Phong.vert", "shaders/Phong.frag");
    phongUvScroll.load("shaders/Phong_uvScroll.vert", "shaders/Phong.frag");
    animationShader.load("shaders/Animating.vert", "shaders/Phong.frag");
    UI.load("shaders/Phong.vert", "shaders/UI.frag");

	passthrough.load("shaders/passthrough.vert", "shaders/passthrough.frag");

    // Geometry Shaders
    geo.load("shaders/passThroughWorld_v.glsl", "shaders/particles_g.glsl", "shaders/passThroughWorld_f.frag");
    // PostProcess Shaders Note: uses geometry to make full screen quad
    passThroughParticles.load("shaders/passThroughWorld_v.glsl", "shaders/quadScreenSpace_g.glsl", "shaders/passthroughparticle.frag");
	passthrough.load("shaders/passthrough.vert", "shaders/passthrough.frag");
    gaussBlurr.load("shaders/passThroughWorld_v.glsl", "shaders/quadScreenSpace_g.glsl", "shaders/gaussianBlur_f.glsl");
    bright.load("shaders/passThroughWorld_v.glsl", "shaders/quadScreenSpace_g.glsl", "shaders/bright_f.glsl");
    composite.load("shaders/passThroughWorld_v.glsl", "shaders/quadScreenSpace_g.glsl", "shaders/bloomComposite_f.glsl");
	postProcessing.load("shaders/passThroughWorld_v.glsl", "shaders/quadScreenSpace_g.glsl", "shaders/postProcessing.frag");


    // Lights + Shader Load END

    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "Lights + Shader Load END time passed: " << deltaTime << std::endl;

    float downFactor = 16;

    // FBO INITIALIZE
    fboScene.createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 1, true);					//Default Scene
    fboBrightPass.createFrameBuffer(WINDOW_WIDTH / downFactor, WINDOW_HEIGHT / downFactor, 1, true);	//Divided by 4 to pixelate
    fboBlur.createFrameBuffer(WINDOW_WIDTH / downFactor, WINDOW_HEIGHT / downFactor, 1, true);					//Blur 
    fboHold.createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 1, true);					//HoldBullet image



                                                                                        //P1RoundWins.mesh = UIObj;
    quad.position = glm::vec3(0.0f, 0.0f, 0.0f);
    emptyQuadMesh = std::make_shared<Mesh>();
    quad.mesh = emptyQuadMesh;
    //    quad.mesh->geoWidth = 30;
    //    quad.mesh->geoHeight = 16;
    quad.mesh->geoWidth = 2;
    quad.mesh->geoHeight = 2;
    quad.mesh->width = WINDOW_WIDTH / downFactor;
    quad.mesh->height = WINDOW_HEIGHT / downFactor;
	quad.loadTexture(TextureType::Diffuse, "textures/postProcessingWarp.png");
    quad.mesh->updateQuad(true, quad.position);

    // -------------------------------------------- CAMERA INITIALIZE --------------------------------------------
    cameraTransform = glm::rotate(cameraTransform, 1.55f, glm::vec3(1.0f, 0.f, 0.f)) * glm::translate(cameraTransform, glm::vec3(0.f, -18.f, -1.3f)); // camera position/rotation
    cameraProjection = glm::perspective(45.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10000.f); // camera perspective, 0.1 for near plane, 10000 for far plane
    cameraProjOrtho = glm::ortho(-15.0f, 15.0f, -8.0f, 7.0f, -100.0f, 100.0f);


    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "FBO load and Camerae Matrix END time passed: " << deltaTime << std::endl;


    //______________________________________________________________________________MAINMENU OBJECT INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> MenuObj = std::make_shared<Mesh>();
    MenuObj->loadFromFile("meshes/MainMenu.obj");
	//______________________________________________________________________________CREDITS OBJECT INITIALIZE______________________________________________________________________________
	std::shared_ptr<Mesh> CreditsObj = std::make_shared<Mesh>();
	CreditsObj->loadFromFile("meshes/credits.obj");
    // -------------------------------------------- MAINMENU INITIALIZE --------------------------------------------
    mainmenu.mesh = MenuObj;
    mainmenu.loadTexture(TextureType::BaseTexture, "textures/start1.png");
    mainmenu.translate = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, 1.5f));
    mainmenu.scale = 0.65f;
    mainmenu.transform = mainmenu.translate * mainmenu.rotate * glm::scale(glm::mat4(), glm::vec3(mainmenu.scale));

	credits.mesh = CreditsObj;
	credits.loadTexture(TextureType::BaseTexture, "textures/credits.png");
	credits.translate = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, 1.5f));
	credits.scale = 0.65f;
	credits.transform = credits.translate * credits.rotate * glm::scale(glm::mat4(), glm::vec3(credits.scale));

    for (int i = 0; i <= 7; i++)
        menuState.push_back(new GameObject(mainmenu));

    menuState[1]->loadTexture(TextureType::BaseTexture, "textures/start2.png");
    menuState[2]->loadTexture(TextureType::BaseTexture, "textures/start3.png");
    menuState[3]->loadTexture(TextureType::BaseTexture, "textures/start4.png");
    menuState[4]->loadTexture(TextureType::BaseTexture, "textures/GameOver.png");
    menuState[5]->loadTexture(TextureType::BaseTexture, "textures/RedWin.png");
    menuState[6]->loadTexture(TextureType::BaseTexture, "textures/BlueWin.png");
	menuState[7]->loadTexture(TextureType::BaseTexture, "textures/loading.png");

    this->draw();

    //Menue INITIALIZE END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "Menue INITIALIZE END time passed: " << deltaTime << std::endl;

    //______________________________________________________________________________SOUND AND FMOD______________________________________________________________________________

    //Place the listener
    se.listener.pos.x = 0.0f;    se.listener.pos.y = 5.0f;    se.listener.pos.z = 0.0f;
    se.setListener(se.listener.pos, se.listener.forward, se.listener.up, se.listener.vel);
    //load a se.sound   //NOTE: se.FileExists will be false if we get a failed load
    std::string wavFile = "Sounds/";
    FMOD_VECTOR tempPos; tempPos.x = 0.0f;    tempPos.y = 0.0f;    tempPos.z = 0.0f;
    FMOD_VECTOR tempVel; tempVel.x = 0.0f; tempVel.y = 0.0f; tempVel.z = 0.0f;
    std::string fileName, searchFile;

    //Load StageSounds

    // main menue music = 0
    fileName = "NonRecorded_VikingBattleTheme_MainMenu.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 30.0f;
    se.sound->setMode(FMOD_LOOP_NORMAL);// make sure it loops
    se.addChannel("main_menue_music", se.sound, tempPos, tempVel, 0);
    se.stageSounds[0].channel->setMode(FMOD_LOOP_NORMAL); // make sure it loops
    
    // in game music = 1
    fileName = "NonRecorded_BattleTrooper_InGame.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 25.0f;
    se.sound->setMode(FMOD_LOOP_NORMAL);
    se.addChannel("game_music", se.sound, tempPos, tempVel, 0);
    se.stageSounds[1].channel->setMode(FMOD_LOOP_NORMAL); 

    // Crowd loop = 2
    fileName = "NonRecorded_ExcitedCrowd.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 10.0f;
    se.sound->setMode(FMOD_LOOP_NORMAL);
    se.addChannel("crowd_loop", se.sound, tempPos, tempVel, 0);
    se.stageSounds[2].channel->setMode(FMOD_LOOP_NORMAL);
    
    // Menu Move up = 3
    fileName = "Recorded_MenuUP.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 7.0f;
    se.addChannel("menue_up", se.sound, tempPos, tempVel, 0);
  
    // Menu Move down = 4
    fileName = "Recorded_MenuDOWN.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 7.0f;
    se.addChannel("menue_down", se.sound, tempPos, tempVel, 0);

    // sheild Bloack 1 = 5
    fileName = "Recorded_ShieldBlock1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 0.0f;
    se.addChannel("sheild_delfect1", se.sound, tempPos, tempVel, 0);
    FMOD::Sound *soundTemp = se.sound;
    
    // sheild Bloack 2 = 6
    fileName = "Recorded_ShieldBlock2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 0.0f;
    se.addChannel("sheild_delfect2", se.sound, tempPos, tempVel, 0);
    FMOD::Sound *soundTemp2 = se.sound;

    // 7 - 8
    se.addChannel("sheild_delfect1.2", soundTemp, tempPos, tempVel, 0); // so both playeres can play same sound
    se.addChannel("sheild_delfect2.2", soundTemp2, tempPos, tempVel, 0); // so both playeres can play same sound

    // gunShot = 9 - 10
    fileName = "NonRecorded_ShootV3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 0.0f;
    se.addChannel("gun_shot", se.sound, tempPos, tempVel, 0);

    fileName = "NonRecorded_ShootV3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 0.0f;
    se.addChannel("gun_shot", se.sound, tempPos, tempVel, 0);

    // gunShot reload = 11 - 12
    fileName = "NonRecorded_ReloadV3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 0.0f;
    se.addChannel("reload", se.sound, tempPos, tempVel, 0);

    fileName = "NonRecorded_ReloadV3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    tempPos.y = 0.0f;
    se.addChannel("reload", se.sound, tempPos, tempVel, 0);

    tempPos.y = 0.0f;

    // bullet wall inpact = 13 
    fileName = "Recorded_WallImpact.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("wall_Impact", se.sound, tempPos, tempVel, 0);


    //Load knight1 Sound
    tempPos.y = 5.0f;

    // Death 1 = 0
    fileName = "Knight1_Death1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("Death1", se.sound, tempPos, tempVel, 2);

    // Death 2 = 1
    fileName = "Knight1_Death2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("Death2", se.sound, tempPos, tempVel, 2);

    // Voice 1 = 2
    fileName = "Knight1_Voice1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("voice1", se.sound, tempPos, tempVel, 2);

    // Voice 2 = 3
    fileName = "Knight1_Voice2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("voice2", se.sound, tempPos, tempVel, 2);

    // Voice 3 = 4
    fileName = "Knight1_Voice3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("voice3", se.sound, tempPos, tempVel, 2);
    tempPos.y = 0.0f;

    // Grunt 1 = 5
    fileName = "Knight1_Grunt1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt1", se.sound, tempPos, tempVel, 2);

    // Grunt 2 = 6
    fileName = "Knight1_Grunt2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt2", se.sound, tempPos, tempVel, 2);

    // Grunt 3 = 7
    fileName = "Knight1_Grunt3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt3", se.sound, tempPos, tempVel, 2);

    // Grunt 4 = 8
    fileName = "Knight1_Grunt4.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt4", se.sound, tempPos, tempVel, 2);

    // Grunt 5 = 9
    fileName = "Knight1_Grunt5.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt5", se.sound, tempPos, tempVel, 2);

    // Grunt 6 = 10
    fileName = "Knight1_Grunt6.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt6", se.sound, tempPos, tempVel, 2);


    //Load knight 3 Sound
    tempPos.y = 5.0f;

    // Death 1 = 0
    fileName = "Knight3_Death1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("Death1", se.sound, tempPos, tempVel, 1);

    // Death 2 = 1
    fileName = "Knight3_Death2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("Death2", se.sound, tempPos, tempVel, 1);

    // Voice 1 = 2
    fileName = "Knight3_Voice1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("voice1", se.sound, tempPos, tempVel, 1);

    // Voice 2 = 3
    fileName = "Knight3_Voice2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("voice2", se.sound, tempPos, tempVel, 1);

    // Voice 3 = 4
    fileName = "Knight3_Voice3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("voice3", se.sound, tempPos, tempVel, 1);
    tempPos.y = 0.0f;

    // Grunt 1 = 5
    fileName = "Knight3_Grunt1.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt1", se.sound, tempPos, tempVel, 1);

    // Grunt 2 = 6
    fileName = "Knight3_Grunt2.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt2", se.sound, tempPos, tempVel, 1);

    // Grunt 3 = 7
    fileName = "Knight3_Grunt3.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt3", se.sound, tempPos, tempVel, 1);

    // Grunt 4 = 8
    fileName = "Knight3_Grunt4.wav";
    searchFile = wavFile + fileName;
    se.fileExists = se.loadSound(searchFile);
    se.addChannel("grunt4", se.sound, tempPos, tempVel, 1);

    //FMOD END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "FMOD END time passed: " << deltaTime << std::endl;


    //______________________________________________________________________________KNIGHT MESH ANIMATION 1 MESH INTIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> knight1Walk_1 = std::make_shared<Mesh>();
    knight1Walk_1->loadFromFile("meshes/KnightTest_A1.obj");

    std::shared_ptr<Mesh> knight1Walk_2 = std::make_shared<Mesh>();
    knight1Walk_2->loadFromFile("meshes/KnightTest_A2.obj");

    std::shared_ptr<Mesh> knight1Walk_3 = std::make_shared<Mesh>();
    knight1Walk_3->loadFromFile("meshes/KnightTest_A3.obj");

    std::shared_ptr<Mesh> knight1Walk_4 = std::make_shared<Mesh>();
    knight1Walk_4->loadFromFile("meshes/KnightTest_A4.obj");

    std::shared_ptr<Mesh> knight1Walk_5 = std::make_shared<Mesh>();
    knight1Walk_5->loadFromFile("meshes/KnightTest_A5.obj");

	std::shared_ptr<Mesh> knight1Walk_6 = std::make_shared<Mesh>();
    knight1Walk_6->loadFromFile("meshes/KnightTest_A6.obj");

	std::shared_ptr<Mesh> knight1Walk_7 = std::make_shared<Mesh>();
    knight1Walk_7->loadFromFile("meshes/KnightTest_A7.obj");

	std::shared_ptr<Mesh> knight1Walk_8 = std::make_shared<Mesh>();
	knight1Walk_8->loadFromFile("meshes/KnightTest_A8.obj");

    //std::shared_ptr<Mesh> knight1Walk_6 = std::make_shared<Mesh>();
    //knight1Walk_6->loadFromFile("meshes/knight5.2_000010.obj");
	//
    //std::shared_ptr<Mesh> knight1Walk_7 = std::make_shared<Mesh>();
    //knight1Walk_7->loadFromFile("meshes/knight5.2_000016.obj");



    // ANIMATION LOAD END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "ANIMATION LOAD END time passed: " << deltaTime << std::endl;

    //______________________________________________________________________________ARENA MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> Arena = std::make_shared<Mesh>();
    Arena->loadFromFile("meshes/arena.obj");

    //______________________________________________________________________________PILLARS MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> Pillar1 = std::make_shared<Mesh>();
    Pillar1->loadFromFile("meshes/Pillar_01.obj");

    std::shared_ptr<Mesh> Pillar2 = std::make_shared<Mesh>();
    Pillar2->loadFromFile("meshes/Pillar_02.obj");

    std::shared_ptr<Mesh> Pillar3 = std::make_shared<Mesh>();
    Pillar3->loadFromFile("meshes/Pillar_02.obj");

    //______________________________________________________________________________BARS MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> Bars = std::make_shared<Mesh>();
    Bars->loadFromFile("meshes/Bars.obj");

    //______________________________________________________________________________BRICK MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> Brick = std::make_shared<Mesh>();
    Brick->loadFromFile("meshes/Brick.obj");

    //______________________________________________________________________________SLAB MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> Slab = std::make_shared<Mesh>();
    Slab->loadFromFile("meshes/path.obj");

    // ______________________________________________________________________________BULLET MESH INITIALIZE______________________________________________________________________________
    Bullet = std::make_shared<Mesh>();
    Bullet->loadFromFile("meshes/BulletNew.obj");

    // ______________________________________________________________________________KNIGHT MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> knight = std::make_shared<Mesh>(); // Define objs that are used
    knight->loadFromFile("meshes/Knight.obj");

    // ______________________________________________________________________________MONKEY HEAD LEFT MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> MonkeyHeadLeft = std::make_shared<Mesh>();
    MonkeyHeadLeft->loadFromFile("meshes/monkeyHeadLeft.obj");

    // ______________________________________________________________________________MONKEY HEAD RIGHT MESH INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> MonkeyHeadRight = std::make_shared<Mesh>();
    MonkeyHeadRight->loadFromFile("meshes/monkeyHeadRight.obj");

    //______________________________________________________________________________UI OBJECT INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> UIObj = std::make_shared<Mesh>();
    UIObj->loadFromFile("meshes/Wins.obj");
    //______________________________________________________________________________SHIELD INITIALIZE______________________________________________________________________________
    std::shared_ptr<Mesh> ShieldObj = std::make_shared<Mesh>();
    ShieldObj->loadFromFile("meshes/Shield.obj");

    // MODEL MESH LOADING END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "MODEL MESH LOADING END time passed: " << deltaTime << std::endl;
    //______________________________________________________________________________Skin Meshing__________________________________________________________________________________________________
    {
        //    std::shared_ptr<Mesh> SkinnedMesh = std::make_shared<Mesh>();;
    //    std::string animationPath = "animations/";
    //    std::string modelsPath = "meshes/";
    //    if (!SkinnedMesh->loadFromFile("meshes/guy.obj"))
    //    {
    //        std::cout << "Model failed to load" << std::endl;
    //        system("pause");
    //        exit(0);
    //    }
        // mesh->loadFromFile(modelsPath + "guy.obj");
    }

    //--- @knight animation Load INITIALIZE ---//	
    knight1Animation.animation.keyFrames.push_back(knight1Walk_1);
    knight1Animation.animation.keyFrames.push_back(knight1Walk_2);
    knight1Animation.animation.keyFrames.push_back(knight1Walk_3);
    knight1Animation.animation.keyFrames.push_back(knight1Walk_4);
    knight1Animation.animation.keyFrames.push_back(knight1Walk_5);
    knight1Animation.animation.keyFrames.push_back(knight1Walk_6);
    knight1Animation.animation.keyFrames.push_back(knight1Walk_7);
	//knight1Animation.animation.keyFrames.push_back(knight1Walk_8);


    //--- @knight animation Load INITIALIZE ---//	
    knight2Animation.animation.keyFrames.push_back(knight1Walk_1);
    knight2Animation.animation.keyFrames.push_back(knight1Walk_2);
    knight2Animation.animation.keyFrames.push_back(knight1Walk_3);
    knight2Animation.animation.keyFrames.push_back(knight1Walk_4);
    knight2Animation.animation.keyFrames.push_back(knight1Walk_5);
    knight2Animation.animation.keyFrames.push_back(knight1Walk_6);
    knight2Animation.animation.keyFrames.push_back(knight1Walk_7);
	//knight2Animation.animation.keyFrames.push_back(knight1Walk_8);

    
	//Knight1 keyframe END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "Knight1 keyframe END time passed: " << deltaTime << std::endl;


    //SPECULUAR LOADING SYNTAX
    // specular on is just a white square. multiplied by light calculations = on, multiplied by black (no specular) = off.
    //.loadTexture(TextureType::Specular, "textures/fullSpecular.png"); // specular on (white texture)
    //.loadTexture(TextureType::Specular, "textures/noSpecular.png");; // specular off (black texture)
	// -------------------------------------------- KNIGHT_1 ANIMATION INITIALIZE --------------------------------------------
	knight1Animation.animation.initialize();
	knight1Animation.mesh = (knight1Animation.animation.keyFrames[knight1Animation.animation.currentFrame]);
	knight1Animation.loadTexture(TextureType::BaseTexture, "textures/Knight_Red.png");
	knight1Animation.loadTexture(TextureType::Normal, "textures/normalMaps/Knight_NormalMap.png");
	knight1Animation.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
	knight1Animation.position = glm::vec3(-8.f, 0.0f, 0.0f);
	knight1Animation.scale = 1.f;
	knight1Animation.translate = glm::translate(knight1Animation.translate, knight1Animation.position);
	knight1Animation.localRotation = 1.5708f; //This is 90 degrees in radians
	knight1Animation.rotate = glm::rotate(glm::mat4(), knight1Animation.localRotation, glm::vec3(0.f, 1.f, 0.f));
	// -------------------------------------------- KNIGHT_2 ANIMATION INITIALIZE --------------------------------------------
	knight2Animation.animation.initialize();
	knight2Animation.mesh = (knight2Animation.animation.keyFrames[knight2Animation.animation.currentFrame]);
	knight2Animation.loadTexture(TextureType::BaseTexture, "textures/Knight_Blue.png");
	knight2Animation.loadTexture(TextureType::Normal, "textures/normalMaps/Knight_NormalMap.png");
	knight2Animation.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
	knight2Animation.position = glm::vec3(-8.f, 0.0f, 0.0f);
	knight2Animation.scale = 1.f;
	knight2Animation.translate = glm::translate(knight2Animation.translate, knight2Animation.position);
	knight2Animation.localRotation = -1.5708f; //This is 90 degrees in radians
	knight2Animation.rotate = glm::rotate(glm::mat4(), knight2Animation.localRotation, glm::vec3(0.f, 1.f, 0.f));

	// -------------------------------------------- KNIGHT_1 INITIALIZE --------------------------------------------
	knight1 = GameObject(knight); //set the object to and OBJ 
	knight1.loadTexture(TextureType::BaseTexture, "textures/Knight_Red.png");
	knight1.position = glm::vec3(-8.f, 0.0f, 0.0f);
	knight1.loadTexture(TextureType::Normal, "textures/normalMaps/Path_CurrentNormalMap.png");
	knight1.translate = glm::translate(knight1.translate, knight1.position); //set position of object
	knight1.localRotation = -1.5708f; //This is 90 degrees in radians
	knight1.rotate = glm::rotate(glm::mat4(), knight1.localRotation, glm::vec3(0.f, 1.f, 0.f));
	knight1.Radius = 1; //USE FOR CIRCLE COLLISIONS
	knight1.OppAngle = glm::pi<float>();
	knight1.currAngle = 0.f;

	// -------------------------------------------- SHIELD_1 INITIALIZE --------------------------------------------
	P1.Shield = GameObject(ShieldObj);
	P1.Shield.loadTexture(TextureType::BaseTexture, "textures/jonWShieldTexture.png");
	P1.Shield.loadTexture(TextureType::Normal, "textures/normalMaps/Shield_NormalMap.png");
	P1.Shield.position = knight1.position;
	P1.Shield.translate = glm::translate(glm::mat4(), P1.Shield.position);
	P1.Shield.Radius = 1.4f;

	// -------------------------------------------- KNIGHT_2 INITIALIZE --------------------------------------------
	knight2 = GameObject(knight); //set the object to and OBJ 
	knight2.loadTexture(TextureType::BaseTexture, "textures/Knight_Blue.png");
	knight2.position = glm::vec3(8.f, 0.0f, 0.0f);
	knight2.loadTexture(TextureType::Normal, "textures/normalMaps/Path_CurrentNormalMap.png");
	knight2.translate = glm::translate(knight2.translate, knight2.position); //set position of object
	knight2.localRotation = 1.5708f; //This is 90 degrees in radians
	knight2.rotate = glm::rotate(glm::mat4(), knight2.localRotation, glm::vec3(0.f, 1.f, 0.f));
	knight2.Radius = 1; //USE FOR CIRCLE COLLISIONS
	knight2.OppAngle = 0.f;
	knight2.currAngle = glm::pi<float>();

	// -------------------------------------------- SHIELD_2 INITIALIZE --------------------------------------------
	P2.Shield = GameObject(ShieldObj);
	P2.Shield.mat.texture = P1.Shield.mat.texture;
	P2.Shield.loadTexture(TextureType::Normal, "textures/normalMaps/Shield_NormalMap.png");

	P2.Shield.position = knight2.position;
	P2.Shield.translate = glm::translate(glm::mat4(), P2.Shield.position);
	P2.Shield.Radius = 1.4f;

	// Charater INITIALIZE END
	updateTime->tick();
	deltaTime = updateTime->getElapsedTimeSeconds();
	totalGameTime += deltaTime; // Update Timer
	std::cout << "Charater INITIALIZE END time passed: " << deltaTime << std::endl;

	// -------------------------------------------- BULLET INITIALIZE --------------------------------------------
	bulletTemplate.mesh = Bullet;
	bulletTemplate.mat.loadTexture(BaseTexture, "textures/red.png");
	bulletTemplate.mat.diffuse = knight1.mat.specular;
	bulletTemplate.mat.specular = knight1.mat.specular;
	bulletTemplate.transform = glm::translate(glm::mat4(), glm::vec3(1000, 0, 1000));
	bulletTemplate.scale = 2.f;
	bulletTemplate.position = glm::vec3(1000, 0, 1000);

	bulletTemplate2 = bulletTemplate;
	bulletTemplate2.mat.loadTexture(BaseTexture, "textures/blue.png");
	for (int i = 0; i < P1.numbOfBullets; i++)
		p1Bullets.push_back(new GameObject(bulletTemplate));

	for (int i = 0; i < P2.numbOfBullets; i++)
		p2Bullets.push_back(new GameObject(bulletTemplate2));

	//Bullet Load INITIALIZE END
	updateTime->tick();
	deltaTime = updateTime->getElapsedTimeSeconds();
	totalGameTime += deltaTime; // Update Timer
	std::cout << "Bullet Load INITIALIZE END time passed: " << deltaTime << std::endl;


				/*
				// -------------------------------------------- KNIGHT_2 ANIMATION INITIALIZE --------------------------------------------
				knight2Animation.animation.initialize();
				knight2Animation.mesh = (knight2Animation.animation.keyFrames[knight2Animation.animation.currentFrame]);
				knight2Animation.loadTexture(TextureType::BaseTexture, "textures/Knight_Blue.png");
				knight2Animation.loadTexture(TextureType::Normal, "textures/normalMaps/Knight_NormalMap.png");
				knight2Animation.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
				knight2Animation.position = glm::vec3(-8.f, 0.0f, 0.0f);
				knight2Animation.scale = 1.0f;
				knight2Animation.translate = glm::translate(knight2Animation.translate, knight2Animation.position);
				knight2Animation.localRotation = -1.5708f; //This is 90 degrees in radians
				knight2Animation.rotate = glm::rotate(glm::mat4(), knight2Animation.localRotation, glm::vec3(0.f, 1.f, 0.f));
				*/
	// -------------------------------------------- ARENA INITIALIZE --------------------------------------------
	arena = GameObject(Arena);
	arena.loadTexture(TextureType::BaseTexture, "textures/arena.png");
	//arena.mat.diffuse = knight1.mat.specular;
	//arena.mat.specular = knight1.mat.specular;
	arena.loadTexture(TextureType::Normal, "textures/normalMaps/Floor_CurrentNormalMap.png"); //Normal Map Texture //normal.jpg - cushion
	arena.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
	arena.rotate = glm::rotate(arena.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	arena.translate = glm::translate(arena.translate, glm::vec3(0.f, -1.f, 0.f));
	arena.scale = 1.8f;
	// -------------------------------------------- BARS INITIALIZE --------------------------------------------
	bars = GameObject(Bars);
	bars.loadTexture(TextureType::BaseTexture, "textures/bars.png");
	//bars.mat.diffuse = knight1.mat.specular;
	//bars.mat.specular = knight1.mat.specular;
	bars.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
	bars.loadTexture(TextureType::Normal, "textures/normalMaps/normal.jpg");
	bars.rotate = glm::rotate(bars.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	bars.translate = glm::translate(bars.translate, glm::vec3(0.f, -1.f, 0.f));
	bars.scale = 1.8f;
	// -------------------------------------------- BRICK INITIALZIE --------------------------------------------
	brick = GameObject(Brick);
	brick.loadTexture(TextureType::BaseTexture, "textures/BrickTextureFinal.png");
	brick.mat.diffuse = knight1.mat.specular;
	brick.mat.specular = knight1.mat.specular;
	brick.rotate = glm::rotate(brick.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	brick.translate = glm::translate(brick.translate, glm::vec3(0.f, -1.f, 0.f));
	brick.scale = 1.8f;
	// -------------------------------------------- SLAB INITIALZIE --------------------------------------------
	slab = GameObject(Slab);
	slab.loadTexture(TextureType::BaseTexture, "textures/path.png");
	slab.loadTexture(TextureType::Normal, "textures/normalMaps/Path_CurrentNormalMap.png");
	slab.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
	//slab.mat.diffuse = knight1.mat.specular;
	//slab.mat.specular = knight1.mat.specular;
	slab.rotate = glm::rotate(slab.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	slab.translate = glm::translate(slab.translate, glm::vec3(0.f, -1.f, 0.f));
	slab.scale = 1.8f;
	// -------------------------------------------- PILLARS INITIALIZE --------------------------------------------
	pillar1 = GameObject(Pillar1);
	pillar1.loadTexture(TextureType::BaseTexture, "textures/pillar1.png");
	pillar1.mat.diffuse = knight1.mat.specular;
	pillar1.mat.specular = knight1.mat.specular;
	pillar1.loadTexture(TextureType::Normal, "textures/normalMaps/flippedNormalPillar1.png");
	pillar1.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");
	pillar1.translate = glm::translate(pillar1.translate, glm::vec3(0.f, 0.f, -5.f));
	pillar1.rotate = glm::rotate(pillar1.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	pillar1.scale = 1.5f;
	pillar1.Radius = 1.0f;

	pillar2 = GameObject(Pillar1);
	pillar2.loadTexture(TextureType::BaseTexture, "textures/pillar2.png");
	pillar2.mat.diffuse = knight1.mat.specular;
	pillar2.mat.specular = knight1.mat.specular;
	pillar2.loadTexture(TextureType::Normal, "textures/normalMaps/flippedNormalPillar2.png");	
	pillar2.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");


	pillar2.translate = glm::translate(pillar2.translate, glm::vec3(0.f, 0.f, 0.f));
	pillar2.rotate = glm::rotate(pillar2.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	pillar2.scale = 1.5f;
	pillar2.Radius = 1.0f;

	pillar3 = GameObject(Pillar1);
	pillar3.loadTexture(TextureType::BaseTexture, "textures/pillar3.png");
	pillar3.mat.diffuse = knight1.mat.specular;
	pillar3.mat.specular = knight1.mat.specular;
	pillar2.loadTexture(TextureType::Normal, "textures/normalMaps/flippedNormalPillar3.png");
	pillar3.loadTexture(TextureType::Diffuse, "textures/TextureRamp.png");


	pillar3.translate = glm::translate(pillar3.translate, glm::vec3(0.f, 0.f, 5.f));
	pillar3.rotate = glm::rotate(pillar3.rotate, 1.5708f, glm::vec3(0.f, 1.f, 0.f));
	pillar3.scale = 1.5f;
	pillar3.Radius = 1.0f;
    // -------------------------------------------- MONKEY HEAD LEFT INITIALIZE --------------------------------------------
    monkeyHeadLeft = GameObject(MonkeyHeadLeft);
    monkeyHeadLeft.loadTexture(TextureType::BaseTexture, "textures/red.png");
    //monkeyHeadLeft.mat.diffuse = knight1.mat.specular;
    //monkeyHeadLeft.mat.specular = knight1.mat.specular;
    monkeyHeadLeft.localRotation = 1.5708f;
    monkeyHeadLeft.rotate = glm::rotate(monkeyHeadLeft.rotate, monkeyHeadLeft.localRotation, glm::vec3(0.f, 1.f, 0.f));
    monkeyHeadLeft.scale = 1.8f;
    // -------------------------------------------- MONKEY HEAD RIGHT INITIALIZE --------------------------------------------
    monkeyHeadRight = GameObject(MonkeyHeadRight);
    monkeyHeadRight.loadTexture(TextureType::BaseTexture, "textures/blue.png");
    //monkeyHeadRight.mat.diffuse = knight1.mat.specular;
    //monkeyHeadRight.mat.specular = knight1.mat.specular;
    monkeyHeadRight.localRotation = 1.5708f;
    monkeyHeadRight.rotate = glm::rotate(monkeyHeadRight.rotate, monkeyHeadRight.localRotation, glm::vec3(0.f, 1.f, 0.f));
    monkeyHeadRight.scale = 1.8f;
    // -------------------------------------------- PARTICLE BASE INITIALIZE --------------------------------------------
    //emitterBase.mesh = partObj;
    emitterBase.localRotation = 1.5708f;
    emitterBase.rotate = glm::rotate(knight1.rotate, emitterBase.localRotation, glm::vec3(0.f, 0.f, 1.f));
    emitterBase.scale = 0.1f;

    //Scene INITIALIZE END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "Scene INITIALIZE END time passed: " << deltaTime << std::endl;

    // -------------------------------------------- P1Wins INITIALIZE --------------------------------------------
    //P1RoundWins.mesh = UIObj;
    P1Health.position = glm::vec3(-13.25, 10.0, 7.f);
    emptyP1HealthMesh = std::make_shared<Mesh>();
    P1Health.mesh = emptyP1HealthMesh;
    P1Health.mesh->geoWidth = 4.f;
    P1Health.mesh->geoHeight = 4.0f;
    P1Health.mesh->updateQuad(true, P1Health.position);

    P1Health.loadTexture(TextureType::BaseTexture, "textures/P1Stock.png");
    P1Health.mat.diffuse = knight1.mat.specular;
    P1Health.mat.specular = knight1.mat.specular;

    emptyP1HealthIcons = std::make_shared<Mesh>();
    P1HealthIcons.mesh = emptyP1HealthIcons;
    P1HealthIcons.mesh->geoWidth = 1.f;
    P1HealthIcons.mesh->geoHeight = 1.0f;
    int numbOfWins = 5;
    P1IconsPos.push_back(new glm::vec3(-1000, 0, 0));
    P1IconsOriginalPos.push_back(glm::vec3(-1000, 0, 0));
    for (int i = 1; i <= numbOfWins; i++)
    {
        P1IconsPos.push_back(new glm::vec3(P1Health.position + glm::vec3(1 * P1HealthIcons.mesh->geoWidth + 1.0f, 0, -i * .75* P1HealthIcons.mesh->geoHeight + 2.5)));
        P1IconsOriginalPos.push_back(glm::vec3(P1Health.position + glm::vec3(1 * P1HealthIcons.mesh->geoWidth + 1.0f, 0, -i * .75* P1HealthIcons.mesh->geoHeight + 2.5)));

    }

    P1HealthIcons.mesh->updateParticlePos(true, P1IconsPos);
    P1HealthIcons.loadTexture(TextureType::BaseTexture, "textures/P1SingleStock.png");
    P1HealthIcons.mat.diffuse = knight1.mat.specular;
    P1HealthIcons.mat.specular = knight1.mat.specular;


    // -------------------------------------------- P2Wins INITIALIZE --------------------------------------------
    P2Health.position = glm::vec3(13.25, 10.0, 7.f);
    emptyP2HealthMesh = std::make_shared<Mesh>();
    P2Health.mesh = emptyP2HealthMesh;
    P2Health.mesh->geoWidth = 4.f;
    P2Health.mesh->geoHeight = 4.0f;
    P2Health.mesh->updateQuad(true, P2Health.position);

    P2Health.loadTexture(TextureType::BaseTexture, "textures/P2Stock.png");
    P2Health.mat.diffuse = knight1.mat.specular;
    P2Health.mat.specular = knight1.mat.specular;

    emptyP2HealthIcons = std::make_shared<Mesh>();
    P2HealthIcons.mesh = emptyP2HealthIcons;
    P2HealthIcons.mesh->geoWidth = 1.0f;
    P2HealthIcons.mesh->geoHeight = 1.0f;
    P2IconsPos.push_back(new glm::vec3(-1000, 0, 0));
    P2IconsOriginalPos.push_back(glm::vec3(-1000, 0, 0));
    for (int i = 1; i <= numbOfWins; i++)
    {
        P2IconsPos.push_back(new glm::vec3(P2Health.position - glm::vec3(1 * P2HealthIcons.mesh->geoWidth + 1.0f, 0.0, i*.75* P2HealthIcons.mesh->geoHeight - 2.5)));
        P2IconsOriginalPos.push_back(glm::vec3(P2Health.position - glm::vec3(1 * P2HealthIcons.mesh->geoWidth + 1.0f, 0.0, i*.75* P2HealthIcons.mesh->geoHeight - 2.5)));
    }
    P2HealthIcons.mesh->updateParticlePos(true, P2IconsPos);
    P2HealthIcons.loadTexture(TextureType::BaseTexture, "textures/P2SingleStock.png");
    P2HealthIcons.mat.diffuse = knight1.mat.specular;
    P2HealthIcons.mat.specular = knight1.mat.specular;


    //UI WINS INITIALIZE END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "UI WINS INITIALIZE END time passed: " << deltaTime << std::endl;



    // load emmiter set Magic.txt
    ReadEmitterFile("defend.txt");

    // store emitters into game's emitter
    for (int i = 0; i < loadedEmitters.size(); i++) {
        emitters.push_back(*loadedEmitters[i]);
    }

    emitterBase.transform = emitterBase.translate * emitterBase.rotate * glm::scale(glm::mat4(), glm::vec3(emitterBase.scale));

    for (int j = 0; j < emitters.size(); j++) {
        for (int i = 0; i < emitters[j].getNumParticles(); i++) {
            particlePos.push_back(new glm::vec3(emitters[j].getParticlePosition(i)));
        }
    }
    emptyParticle = std::make_shared<Mesh>();
    partPosMeshAcess.mesh = emptyParticle;
    partPosMeshAcess.mesh->geoWidth = 10.25f;
    partPosMeshAcess.mesh->geoHeight = 10.25f;
    partPosMeshAcess.mesh->updateParticlePos(true, particlePos);
    partPosMeshAcess.loadTexture(TextureType::BaseTexture, "textures/stolen_sand.png");
    partPosMeshAcess.mat.diffuse = knight1.mat.specular;
    partPosMeshAcess.mat.specular = knight1.mat.specular;

    while (!loadedEmitters.empty()) {
        loadedEmitters.pop_back();
    }

    // store emitters into game's emitter for P1
    for (int i = 0; i < p1Bullets.size(); i++) {
        loadedEmitters.push_back(new ParticleEmitter());
        trailEmitterBase1.push_back(new GameObject());
    }
    for (int i = 0; i < loadedEmitters.size(); i++) {
        trailEmitter1.push_back(*loadedEmitters[i]);
    }
    for (int i = 0; i < trailEmitter1.size(); i++) {
       trailEmitterBase1[i]->transform = knight1.transform;
    }

    for (int j = 0; j < trailEmitter1.size(); j++) {
        std::vector<glm::vec3*> trailEmitterPos1;
        for (int i = 0; i < trailEmitter1[j].getNumParticles(); i++) {
            trailEmitterPos1.push_back(new glm::vec3(trailEmitter1[j].getParticlePosition(i)));
        }
        trailEmitterPosHOLDER.push_back(trailEmitterPos1);
    }
    emptytrailMeshP1_1 = std::make_shared<Mesh>(); emptytrailMeshP1_2 = std::make_shared<Mesh>(); emptytrailMeshP1_3 = std::make_shared<Mesh>(); emptytrailMeshP1_4 = std::make_shared<Mesh>(); emptytrailMeshP1_5 = std::make_shared<Mesh>();
    trailEmitterBase1[0]->mesh = emptytrailMeshP1_1; trailEmitterBase1[1]->mesh = emptytrailMeshP1_2; trailEmitterBase1[2]->mesh = emptytrailMeshP1_3; trailEmitterBase1[3]->mesh = emptytrailMeshP1_4;trailEmitterBase1[4]->mesh = emptytrailMeshP1_5;

    for (int i = 0; i < trailEmitterBase1.size(); i++) {
        trailEmitterBase1[i]->mesh->geoWidth = 0.4f;
        trailEmitterBase1[i]->mesh->geoHeight = 0.4f;
        trailEmitterBase1[i]->mesh->updateParticlePos(true, trailEmitterPosHOLDER[i]);
        trailEmitterBase1[i]->loadTexture(TextureType::BaseTexture, "textures/red_trail.png");
        trailEmitterBase1[i]->mat.diffuse = knight1.mat.specular;
        trailEmitterBase1[i]->mat.specular = knight1.mat.specular;
    }


    while (!loadedEmitters.empty()) {
        loadedEmitters.pop_back();
    }

    // store emitters into game's emitter for P2
    for (int i = 0; i < p2Bullets.size(); i++) {
        loadedEmitters.push_back(new ParticleEmitter());
        trailEmitterBase2.push_back(new GameObject());
    }
    for (int i = 0; i < loadedEmitters.size(); i++) {
        trailEmitter2.push_back(*loadedEmitters[i]);
        trailEmitterBase2[i]->transform = knight2.transform;
    }
    for (int j = 0; j < trailEmitter2.size(); j++) {
        std::vector<glm::vec3*> trailEmitterPos2;
        for (int i = 0; i < trailEmitter2[j].getNumParticles(); i++) {
            trailEmitterPos2.push_back(new glm::vec3(trailEmitter2[j].getParticlePosition(i)));
        }
        trailEmitterPos2HOLDER2.push_back(trailEmitterPos2);
    }
    emptytrailMeshP2_1 = std::make_shared<Mesh>(); emptytrailMeshP2_2 = std::make_shared<Mesh>(); emptytrailMeshP2_3 = std::make_shared<Mesh>(); emptytrailMeshP2_4 = std::make_shared<Mesh>(); emptytrailMeshP2_5 = std::make_shared<Mesh>();
    trailEmitterBase2[0]->mesh = emptytrailMeshP2_1; trailEmitterBase2[1]->mesh = emptytrailMeshP2_2; trailEmitterBase2[2]->mesh = emptytrailMeshP2_3; trailEmitterBase2[3]->mesh = emptytrailMeshP2_4;trailEmitterBase2[4]->mesh = emptytrailMeshP2_5;

    for (int i = 0; i < trailEmitterBase2.size(); i++) {
        trailEmitterBase2[i]->mesh->geoWidth = 0.4f;
        trailEmitterBase2[i]->mesh->geoHeight = 0.4f;
        trailEmitterBase2[i]->mesh->updateParticlePos(true, trailEmitterPos2HOLDER2[i]);
        trailEmitterBase2[i]->loadTexture(TextureType::BaseTexture, "textures/blue_trail.png");
        trailEmitterBase2[i]->mat.diffuse = knight1.mat.specular;
        trailEmitterBase2[i]->mat.specular = knight1.mat.specular;
    }


    // PARTICLE POS AND INITIALZE END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "PARTICLE POS AND INITIALZE END time passed: " << deltaTime << std::endl;

    {

    // SCENE INITIALIZE
    arena.transform = arena.translate * arena.rotate * glm::scale(glm::mat4(), glm::vec3(arena.scale)); arena.uvScroll = 0.0f;
    arena.keyframeController.doesSpeedControl = true; arena.catmullT0 = glm::vec3(-1.0f); arena.catmullT1 = glm::vec3(2.0f);
    arena.keyframeController.addKey(glm::vec3(0.0f));
    arena.keyframeController.addKey(glm::vec3(1.0f));


    arena.keyframeController.calculateLookupTable(6);
    arena.keyframeController.curvePlaySpeed = 10.0f;
//
    bars.transform = bars.translate * bars.rotate * glm::scale(glm::mat4(), glm::vec3(bars.scale));
    brick.transform = brick.translate * brick.rotate * glm::scale(glm::mat4(), glm::vec3(brick.scale));
    slab.transform = slab.translate * slab.rotate * glm::scale(glm::mat4(), glm::vec3(brick.scale));
    pillar1.transform = pillar1.translate * pillar1.rotate * glm::scale(glm::mat4(), glm::vec3(pillar1.scale));
	pillar2.transform = pillar2.translate * pillar2.rotate * glm::scale(glm::mat4(), glm::vec3(pillar2.scale));
	pillar3.transform = pillar3.translate * pillar3.rotate * glm::scale(glm::mat4(), glm::vec3(pillar3.scale));
	monkeyHeadLeft.transform = monkeyHeadLeft.translate * monkeyHeadLeft.rotate * glm::scale(glm::mat4(), glm::vec3(monkeyHeadLeft.scale));
    monkeyHeadRight.transform = monkeyHeadRight.translate * monkeyHeadRight.rotate * glm::scale(glm::mat4(), glm::vec3(monkeyHeadRight.scale));

    arena.position = glm::vec3(arena.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    bars.position = glm::vec3(bars.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    brick.position = glm::vec3(brick.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    slab.position = glm::vec3(slab.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    pillar1.position = glm::vec3(pillar1.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pillar2.position = glm::vec3(pillar2.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pillar3.position = glm::vec3(pillar3.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    monkeyHeadLeft.position = glm::vec3(monkeyHeadLeft.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    monkeyHeadRight.position = glm::vec3(monkeyHeadRight.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // CAMREA AND OBJ TRANSFORMATION END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "CAMREA AND OBJ TRANSFORMATION END time passed: " << deltaTime << std::endl;

    }
    //Plays the sounds
    //System->playSound(sound1, nullptr, false, &channel1); //For BG music shot sound ->playSound inside button switch

    // INUT FINAL INITALIZE END
    updateTime->tick();
    deltaTime = updateTime->getElapsedTimeSeconds();
    totalGameTime += deltaTime; // Update Timer
    std::cout << "time passed: " << totalGameTime << std::endl;
    totalGameTime = 0.0f;// reset totalGameTime



    gs = PressStart; // NOTE: set this to main menue later
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
    totalGameTime += deltaTime; // Update Timer


   
 //   arena.uvScroll += deltaTime/3.0f; // This is to only go forwards in a loop
 // if (arena.uvScroll > 1) {arena.uvScroll = 0.0f;}
    if (gs == PressStart)
    {
        if (se.stageSounds[0].startSound == false )
        se.playSound(0, 0); // Note: uncomment to play main menue song


        Input::XBoxInput XBoxController;
        XBoxController.DownloadPackets(1);

        if (XBoxController.GetConnected(0))
            if (XBoxController.GetButton(0, Input::Button::Start)) {
                se.playSound(2, 0);
                se.stageSounds[2].channel->setMode(FMOD_LOOP_OFF);
                gs = MainMenu;
            }
    }
    else if (gs == MainMenu)
    {
        Input::XBoxInput XBoxController;
        XBoxController.DownloadPackets(1);

        if (XBoxController.GetConnected(0))
        {

            if ((XBoxController.GetButton(0, Input::Button::DPadDown) || XBoxController.GetButton(0, Input::Button::DPadUp)) && menuButtonDelay > 0.25)
            {
                if ((XBoxController.GetButton(0, Input::Button::DPadDown)) && menuButtonDelay > 0.25)
                {
                    se.playSound(4, 0);
                }
                else if ((XBoxController.GetButton(0, Input::Button::DPadDown)) && menuButtonDelay > 0.25)
                {
                    se.playSound(3, 0);
                }

                playOrQuit = !playOrQuit;
                menuButtonDelay = 0;
            }
            menuButtonDelay += deltaTime;
            if (XBoxController.GetButton(0, Input::Button::A))
            {
                if (playOrQuit == false)
                {
                    se.fadeOutChannel(0, 0, 50000);
                    se.playSound(2, 0);

                    if (se.stageSounds[1].startSound == false)
                        se.playSound(1, 0);
                    se.fadeinChannel(1, 0, 100000);
                    gs = GameLoop;
                }
                else
                    exit(1);
            }
        }
    }
    else if (gs == GameOver)
    {
		Input::XBoxInput XBoxController;
		XBoxController.DownloadPackets(1);
		if (XBoxController.GetButton(0, Input::Button::A))
			GameOverDelay = 12;

        GameOverDelay += deltaTime;
        if (GameOverDelay > 12)
        {
            GameOverDelay = 0;
            se.stageSounds[1].startSound = false;
            se.stageSounds[1].soundEnded = true;
            se.stageSounds[1].channel->setPaused(true);
            se.stageSounds[1].channel->stop();

            se.fadeinChannel(0, 0, 100000);
            gs = GameLoop;
            gs = PressStart;
        }
    }
    else if (gs == GameLoop)
    {
        if(se.stageSounds[0].startSound)
        songTransitionDelay += deltaTime;

        if (songTransitionDelay >= 5) {
            se.stageSounds[0].startSound = false;
            se.stageSounds[0].soundEnded = true;
            se.stageSounds[0].channel->setPaused(true);
            se.stageSounds[0].channel->stop();
        }

        if (doDeCast)
        {
            //update timer so we have correct delta time since last update	
            timer += deltaTime;
            if (timer > 1) {
                if (splineCounter <= 0) //0 will play once
                                        //2 will play 3 times etc
                    splineTime += deltaTime * 0.2; //update spline time, multiple the dt by a fraction to slow down
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

        float speed = 8.f;
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


        if (knight2Animation.animation.t > 1.f)
        {
            knight2Animation.animation.currentFrame++;

            if (knight2Animation.animation.keyFrames.size() <= knight2Animation.animation.currentFrame)
            {
                knight2Animation.animation.currentFrame = 0;
            }
            knight2Animation.animation.t = 0;
        }
        knight2Animation.mesh = (knight2Animation.animation.keyFrames[knight2Animation.animation.currentFrame]);


        // -------------------------------------------- Controller Input --------------------------------------------
        Input::XBoxInput XBoxController;
        XBoxController.DownloadPackets(2); // the first parameter is the number of controllers connected

		//BOOL START
		if (freeze == false)
		{

			if (XBoxController.GetConnected(0)) // Player 1 Controls BEGINS
			{
				// -------------------------------------------- GET CONTROLS PLAYER 1 --------------------------------------------
				Input::Stick lStick1, rStick1;
				XBoxController.GetSticks(0, lStick1, rStick1);
				float lTrig1, rTrig1;
				XBoxController.GetTriggers(0, lTrig1, rTrig1);

				// -------------------------------------------- LEFT THUMB STICK PLAYER 1 --------------------------------------------
				knight1.Movement(lStick1, pillar1, pillar2, pillar3);


				if (lStick1.xAxis > 0 || lStick1.xAxis < 0 || lStick1.yAxis > 0 || lStick1.yAxis < 0) // Check Magnitude of Stick and
				{
					knight1Animation.animation.t += deltaTime * speed;
				}
				//knight1Animation.translate = knight1.translate;
				//knight1Animation.rotate = -knight1.rotate;

				// -------------------------------------------- RIGHT THUMB STICK PLAYER 1 --------------------------------------------
				P1.GetRightStick(&knight1, rStick1, lStick1);
				P1.Shield.rotate = glm::rotate(knight1.rotate, 0.f, glm::vec3(0.0f, 1.0f, 0.0f));

				// -------------------------------------------- RIGHT TRIGGER PLAYER 1 --------------------------------------------
				*p1Bullets[P1.BulletNum] = bulletTemplate; // since there are no pointers in bullet obj, we can have loaded bullet var = template var

				if (rTrig1 == 1)
				{
					P1.Shoot(&knight1, p1Bullets, rStick1); // -------------------------------------------- SHOOT BULLET --------------------------------------------
					P1.IterateBulletNum(); // If once1 is true, bulletNum changes to the next num in line
					if (P1.shouldBulletPlaySound) {
                        FMOD_VECTOR newPos;
                        newPos.x = -knight1Animation.position.x;newPos.y = knight1Animation.position.y;newPos.z = knight1Animation.position.z;
                        se.stageSounds[9].soundPos = newPos;
                        se.stageSounds[9].channel->set3DAttributes(&se.stageSounds[9].soundPos, &se.stageSounds[9].soundVel);
                        se.stageSounds[11].channel->set3DAttributes(&se.stageSounds[9].soundPos, &se.stageSounds[9].soundVel);

                        se.playSound(9, 0); //gunshot
                        se.pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, float(rand() % 1000) / 999.0f*1.5f + 0.7f);
                        se.stageSounds[9].channel->addDSP(0, se.pitchShift);

                        se.playSound(11, 0); //reload
                        P1.shouldBulletPlaySound = false;
					}
				}
				P1.BulletCooldown(deltaTime); // Bullet Cool Down
				if (rTrig1 == 0) // If RT is not pressed the bulletNum changes once
					P1.once = true;

				P1.UpdatePrevStick(rStick1);// If magnitude of rStick is greater than 10%, update prevStick

                for (int i = 0; i < p1Bullets.size(); i++) // change Bullets into function call variable
                {
                    if (P1.BulletBounceNum[i] <= 3) {

                    }
                }
				// -------------------------------------------- Bullet Pillar Collision --------------------------------------------
				// A Shit Ton of Bullshit Hardcoded Pillar Collision w/ Bullets is below.
				P1.BulletPillarCollision(p1Bullets, pillar1, pillar2, pillar3);
				//-------------------------------------------- Outer Walls Bullet Collision Check --- PLAYER 1 --------------------------------------------
				P1.BulletOuterWallsCollision(p1Bullets);
				//-------------------------------------------- Bullet Bounce Num Check (Bullets are Destroyed after 3 bounces) --- PLAYER 1 ---------------
				P1.CheckBulletBounceLimit(p1Bullets, 3);

                if(P1.shouldBulletBouncePlaySound) {
                    FMOD_VECTOR newPos;
                    newPos.x = -knight1Animation.position.x;newPos.y = 0;newPos.z = knight1Animation.position.z;
                    se.stageSounds[13].soundPos = newPos;
                    se.stageSounds[13].channel->set3DAttributes(&se.stageSounds[13].soundPos, &se.stageSounds[13].soundVel);
                    se.stageSounds[13].channel->set3DAttributes(&se.stageSounds[13].soundPos, &se.stageSounds[13].soundVel);
                    se.playSound(13, 0);
                    se.pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, float(rand() % 1000) / 999.0f*1.5f + 0.5f);
                    se.stageSounds[13].channel->addDSP(0, se.pitchShift);
                    P1.shouldBulletBouncePlaySound = false;
                }

			}// Player 1 Controls ENDS

			if (XBoxController.GetConnected(1)) // Player 2 Controls BEGINS
			{
				// -------------------------------------------- GET CONTROLS PLAYER 2 --------------------------------------------
				Input::Stick lStick2, rStick2;
				XBoxController.GetSticks(1, lStick2, rStick2);
				float lTrig2, rTrig2;
				XBoxController.GetTriggers(1, lTrig2, rTrig2);

				// -------------------------------------------- LEFT THUMB STICK PLAYER 2 --------------------------------------------
				knight2.Movement(lStick2, pillar1, pillar2, pillar3);
				if (lStick2.xAxis > 0 || lStick2.xAxis < 0 || lStick2.yAxis > 0 || lStick2.yAxis < 0) // Check Magnitude of Stick and
				{
					knight2Animation.animation.t += deltaTime * speed;
				}
				// -------------------------------------------- RIGHT THUMB STICK PLAYER 2 --------------------------------------------
				P2.GetRightStick(&knight2, rStick2, lStick2);
				P2.Shield.rotate = glm::rotate(knight2.rotate, 0.f, glm::vec3(0.0f, 1.0f, 0.0f));

				// -------------------------------------------- RIGHT TRIGGER PLAYER 2 --------------------------------------------
				*p2Bullets[P2.BulletNum] = bulletTemplate2; // since there are no pointers in bullet obj, we can have loaded bullet var = template var

				if (rTrig2 == 1)
				{
					P2.Shoot(&knight2, p2Bullets, rStick2);
					P2.IterateBulletNum(); // If once1 is true, bulletNum changes to the next num in line
					if (P2.shouldBulletPlaySound) {

                        FMOD_VECTOR newPos;
                        newPos.x = -knight2Animation.position.x;newPos.y = knight2Animation.position.y;newPos.z = knight2Animation.position.z;
                        se.stageSounds[10].soundPos = newPos;
                        se.stageSounds[10].channel->set3DAttributes(&se.stageSounds[10].soundPos, &se.stageSounds[10].soundVel);
                        se.stageSounds[12].channel->set3DAttributes(&se.stageSounds[10].soundPos, &se.stageSounds[10].soundVel);

                        se.playSound(10, 0); //gunshot
                        se.pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, se.Random(0.65, 1.4));
                        se.stageSounds[10].channel->addDSP(0, se.pitchShift);
                        se.playSound(12, 0); //reload
                        P2.shouldBulletPlaySound = false;
					}
				}
				P2.BulletCooldown(deltaTime); // Bullet Cool Down

				if (rTrig2 == 0) // If RT is not pressed the bulletNum changes once
					P2.once = true;

				P2.UpdatePrevStick(rStick2); // If magnitude of rStick is greater than 10%, update prevStick

				// -------------------------------------------- BULLET PILLAR COLLISIONS --------------------------------------------
				P2.BulletPillarCollision(p2Bullets, pillar1, pillar2, pillar3);
				// -------------------------------------------- Outer Walls/Pillar Bullet Collision Check --- PLAYER 2  --------------------------------------------
				P2.BulletOuterWallsCollision(p2Bullets);
				// Bullet Bounce Num Check (Bullets are Destroyed after 3 bounces) 
				P2.CheckBulletBounceLimit(p2Bullets, 3);

                if (P2.shouldBulletBouncePlaySound) {
                    FMOD_VECTOR newPos;
                    newPos.x = -knight2Animation.position.x;newPos.y = 0;newPos.z = knight2Animation.position.z;
                    se.stageSounds[13].soundPos = newPos;
                    se.stageSounds[13].channel->set3DAttributes(&se.stageSounds[13].soundPos, &se.stageSounds[13].soundVel);
                    se.stageSounds[13].channel->set3DAttributes(&se.stageSounds[13].soundPos, &se.stageSounds[13].soundVel);
                    se.playSound(13, 0);
                    se.pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, se.Random(0.65,1.4));
                    se.stageSounds[13].channel->addDSP(0, se.pitchShift);
                    P2.shouldBulletBouncePlaySound = false;
                }

			}// Player 2 Controls ENDS


			// -------------------------------------------- Shield Collision Player 1 --------------------------------------------
			P1.ShieldCollision(P2, &knight1, p2Bullets); // Check for P1's sheild against P1, using Knight1 agasint P2's Bullets
		
            for (int i = 0; i < p2Bullets.size(); i++)
			{
				if (P1.bulletHitSheild[i] == false)
				{
					if (glm::distance(p2Bullets[i]->position, P1.Shield.position) < P1.Shield.Radius)
					{
                        int sheildRand = se.RandomInt(5, 6);
                        int gruntRand = se.RandomInt(5, 8);
                        FMOD_VECTOR newPos;
                        newPos.x = -knight1Animation.position.x;newPos.y = knight1Animation.position.y;newPos.z = knight1Animation.position.z;
                        se.stageSounds[sheildRand].soundPos = newPos;
                        se.stageSounds[sheildRand].channel->set3DAttributes(&se.stageSounds[sheildRand].soundPos, &se.stageSounds[sheildRand].soundVel);
                        se.stageSounds[gruntRand].channel->set3DAttributes(&se.stageSounds[sheildRand].soundPos, &se.stageSounds[sheildRand].soundVel);

						se.playSound(sheildRand, 0); // sheild
                        se.playSound(gruntRand, 1); // grunts

					}
				}
			}
			// -------------------------------------------- Shield Collision Player 2 --------------------------------------------
			P2.ShieldCollision(P1, &knight2, p1Bullets);
			for (int i = 0; i < p1Bullets.size(); i++)
			{
				if (P2.bulletHitSheild[i] == false)
				{
					if (glm::distance(p1Bullets[i]->position, P2.Shield.position) < P2.Shield.Radius)
					{
                        int sheildRand = se.RandomInt(7, 8);
                        int gruntRand = se.RandomInt(6, 10);

                        FMOD_VECTOR newPos;
                        newPos.x = -knight2Animation.position.x;newPos.y = knight2Animation.position.y;newPos.z = knight2Animation.position.z;
                        se.stageSounds[sheildRand].soundPos = newPos;
                        se.stageSounds[sheildRand].channel->set3DAttributes(&se.stageSounds[sheildRand].soundPos, &se.stageSounds[sheildRand].soundVel);
                        se.stageSounds[gruntRand].channel->set3DAttributes(&se.stageSounds[sheildRand].soundPos, &se.stageSounds[sheildRand].soundVel);

                        se.playSound(sheildRand, 0);// sheild
                        se.playSound(gruntRand, 2); // grunts
                    }
				}
			}
            // -------------------------------------------- Player 1 Death --------------------------------------------

			for (int i = 0; i < p2Bullets.size(); i++) {
				if (glm::length(P2.BulletMoveVal[i]) != 0.0f)
					if (glm::distance(p2Bullets[i]->position, knight1.position) < knight1.Radius) // CIRCLE COLLISION CHECK
					{
						freeze = true;
						freezeP2 = true;
                        se.playSound(se.RandomInt(0, 1), 1); // death lines
                        trailEmitter1[i].boxOn = true;
                        trailEmitter1[i].taretgOn = true;
                        trailEmitter1[i].velocity0 = - glm::vec3(P2.BulletMoveVal[i].x, 0.0, P2.BulletMoveVal[i].y);
                        trailEmitter1[i].velocity1 = -glm::vec3(P2.BulletMoveVal[i].x, 0.0, P2.BulletMoveVal[i].y) * 2.0f;

                        trailEmitter1[i].targetPos = p2Bullets[i]->position - glm::vec3(P2.BulletMoveVal[i].x, 0.0, P2.BulletMoveVal[i].y);
                        trailEmitter1[i].targetStrength = 50.0f;
					}
			}
			// -------------------------------------------- Player 2 Death --------------------------------------------

			for (int i = 0; i < p1Bullets.size(); i++) {
				if (glm::length(P1.BulletMoveVal[i]) != 0.0f)
					if (glm::distance(p1Bullets[i]->position, knight2.position) < knight2.Radius) // CIRCLE COLLISION CHECK
					{
						freeze = true;
						freezeP1 = true;
                        se.playSound(se.RandomInt(0, 1), 2); // death lines
                        trailEmitter2[i].boxOn = true;
                        trailEmitter2[i].taretgOn = true;
                        trailEmitter2[i].velocity0 = -glm::vec3(P1.BulletMoveVal[i].x, 0.0, P1.BulletMoveVal[i].y);
                        trailEmitter2[i].velocity1 = -glm::vec3(P1.BulletMoveVal[i].x, 0.0, P1.BulletMoveVal[i].y) * 2.0f;

                        trailEmitter2[i].targetPos = p1Bullets[i]->position - glm::vec3(P1.BulletMoveVal[i].x, 0.0, P1.BulletMoveVal[i].y);
                        trailEmitter2[i].targetStrength = 50.0f;
					}
			}
			// BOOL END
		}

		if (freeze == true)
		{
			if (freezeTimer >= 1.5)
			{
                for (int i = 0; i < p1Bullets.size(); i++) {
                    trailEmitter1[i].boxOn = false;
                    trailEmitter1[i].taretgOn = false;
                    trailEmitter1[i].targetPos = glm::vec3(0.0);
                    trailEmitter1[i].targetStrength = 0.0f;
                    trailEmitter1[i].velocity0 = glm::vec3(0.0f);
                    trailEmitter1[i].velocity1 = glm::vec3(0.0f);

                    trailEmitter2[i].boxOn = false;
                    trailEmitter2[i].taretgOn = false;
                    trailEmitter2[i].targetPos = glm::vec3(0.0);
                    trailEmitter2[i].targetStrength = 0.0f;
                    trailEmitter2[i].velocity0 = glm::vec3(0.0f);
                    trailEmitter2[i].velocity1 = glm::vec3(0.0f);
                }
				freezeTimer = 0;
				freeze = false;
				if (freezeP1 == true)
				{
					P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
					P1.Shield.position = knight1.position;
					P1.Shield.translate = glm::translate(glm::mat4(), P1.Shield.position);
					P1.Shield.Radius = 1.4f;
					P2.Shield.position = knight2.position;
					P2.Shield.translate = glm::translate(glm::mat4(), P2.Shield.position);
					P2.Shield.Radius = 1.4f;
					knight1.currAngle = glm::pi<float>();
					knight2.currAngle = glm::pi<float>();
					P1.PrevStickX = 1;
					P1.PrevStickY = 0;
					P2.PrevStickX = 0;
					P2.PrevStickY = 0;
					freezeP1 = false;
				}
				else if (freezeP2 == true)
				{
					P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);

					P1.Shield.position = knight1.position;
					P1.Shield.translate = glm::translate(glm::mat4(), P1.Shield.position);
					P1.Shield.Radius = 1.4f;
					P2.Shield.position = knight2.position;
					P2.Shield.translate = glm::translate(glm::mat4(), P2.Shield.position);
					P2.Shield.Radius = 1.4f;
					knight1.currAngle = glm::pi<float>();
					knight2.currAngle = glm::pi<float>();
					P1.PrevStickX = 1;
					P1.PrevStickY = 0;
					P2.PrevStickX = 0;
					P2.PrevStickY = 0;
					freezeP2 = false;
				}
				
			}
			freezeTimer += deltaTime;
		}

        // -------------------------------------------- Translate if bullets are supposed to move --------------------------------------------
        if (P1.BulletMove == true){
            for (int i = 0; i < p1Bullets.size(); i++)
            {
                p1Bullets[i]->translate = glm::translate(p1Bullets[i]->translate, glm::vec3(P1.BulletMoveVal[i].x, 0.f, P1.BulletMoveVal[i].y));
            }
        }
        if (P2.BulletMove == true) {
            for (int i = 0; i < p1Bullets.size(); i++)
            {
                p2Bullets[i]->translate = glm::translate(p2Bullets[i]->translate, glm::vec3(P2.BulletMoveVal[i].x, 0.f, P2.BulletMoveVal[i].y));
            }
        }

        if (doParenting == true)
            cameraTransform = glm::rotate(glm::mat4(1.0f), 1.5f, glm::vec3(1.0f, 0.f, 0.f)) * glm::translate(glm::mat4(1.0f), glm::vec3(knight1.position.x * -1, -18.f, -2.f));

        // ---------------------- F = T * R * S ----------------------
        P1.Shield.transform = glm::translate(glm::mat4(), P1.Shield.position) * P1.Shield.rotate * glm::scale(glm::mat4(), glm::vec3(P1.Shield.scale));
        P2.Shield.transform = glm::translate(glm::mat4(), P2.Shield.position) * P2.Shield.rotate * glm::scale(glm::mat4(), glm::vec3(P2.Shield.scale));
        //P1.Shield.transform = P1.Shield.translate * P1.Shield.rotate * P1.Shield.scale;
        //P2.Shield.transform = P2.Shield.translate * P2.Shield.rotate * P2.Shield.scale;

        knight1.transform = knight1.translate * knight1.rotate * glm::scale(glm::mat4(), glm::vec3(knight1.scale));
        knight2.transform = knight2.translate * knight2.rotate * glm::scale(glm::mat4(), glm::vec3(knight2.scale));     
        knight1Animation.transform = glm::translate(knight1.translate, glm::vec3(0.0f,1.0f,0.0f)) * glm::rotate(knight1.rotate, glm::pi<float>(), glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(), glm::vec3(knight1Animation.scale));
        knight2Animation.transform = glm::translate(knight2.translate, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(knight2.rotate, glm::pi<float>(), glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(), glm::vec3(knight2Animation.scale));

        monkeyHeadLeft.transform = glm::translate(glm::mat4(), P1.Shield.position) * monkeyHeadLeft.rotate * glm::scale(glm::mat4(), glm::vec3(monkeyHeadLeft.scale));
        monkeyHeadRight.transform = glm::translate(glm::mat4(), P2.Shield.position) * monkeyHeadRight.rotate * glm::scale(glm::mat4(), glm::vec3(monkeyHeadRight.scale));

        //skinnedGameObject1.drawMatrix();
        //Skinned mesh Transform
        //skinnedGameObject1.transform = glm::translate(glm::mat4(), glm::vec3(1.0f));

        P1RoundWins.transform = P1RoundWins.translate * P1RoundWins.rotate * glm::scale(glm::mat4(), glm::vec3(P1RoundWins.scale));
        P2RoundWins.transform = P2RoundWins.translate * P2RoundWins.rotate * glm::scale(glm::mat4(), glm::vec3(P2RoundWins.scale));
   //   
        knight1.position = glm::vec3(knight1.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        knight2.position = glm::vec3(knight2.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        knight1Animation.position = glm::vec3(knight1Animation.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        knight2Animation.position = glm::vec3(knight2Animation.transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        for (int i = 0; i < p1Bullets.size(); i++)
        {
            p1Bullets[i]->transform = p1Bullets[i]->translate * p1Bullets[i]->rotate * glm::scale(glm::mat4(), glm::vec3(p1Bullets[i]->scale));
            p2Bullets[i]->transform = p2Bullets[i]->translate * p2Bullets[i]->rotate * glm::scale(glm::mat4(), glm::vec3(p2Bullets[i]->scale));

            p1Bullets[i]->position = glm::vec3(p1Bullets[i]->transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            p2Bullets[i]->position = glm::vec3(p2Bullets[i]->transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            trailEmitter1[i].emitterPosition = p1Bullets[i]->position;
            trailEmitter1[i].update(deltaTime);

            trailEmitter2[i].emitterPosition = p2Bullets[i]->position;
            trailEmitter2[i].update(deltaTime);
        }


		
		// Particle Movement Update
		for (int i = 0; i < emitters.size(); i++) {
			emitters[i].update(deltaTime);
		}
        // update particle pos array
        int itr = 0;
        for (int j = 0; j < emitters.size(); j++) {
            for (int i = 0; i < emitters[j].getNumParticles(); i++) {

                *particlePos[itr] = emitters[j].getParticlePosition(i) / 50.0f + glm::vec3(-5.0, 10.f, 0.0f) ;
                itr++;
            }
        }
        // update trail pos array
        for (int j = 0; j < trailEmitter1.size(); j++) {
            for (int i = 0; i < trailEmitter1[j].getNumParticles(); i++) {

                *trailEmitterPosHOLDER[j][i] = trailEmitter1[j].getParticlePosition(i) + glm::vec3(0.0, 2.f, 0.0f);
//                std::cout << " part   x: " << trailEmitterPosHOLDER[j][i]->x << " y: " << trailEmitterPosHOLDER[j][i]->y << " z: " << trailEmitterPosHOLDER[j][i]->z << std::endl;
            }
        }
        // update trail pos array
        for (int j = 0; j < trailEmitter2.size(); j++) {
            for (int i = 0; i < trailEmitter2[j].getNumParticles(); i++) {

                *trailEmitterPos2HOLDER2[j][i] = trailEmitter2[j].getParticlePosition(i) + glm::vec3(0.0, 2.f, 0.0f);
            }

        }
    }
//    cameraTransform = glm::lookAt( glm::vec3(0,10,0), trailEmitter1[0].getParticlePosition(0), glm::vec3(0, 1, 0));

    se.update(deltaTime); // update per frame

}


void Game::draw()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    // -------------------------------------------- DRAW CALLS --------------------------------------------
    if (gs == GameLoop)
    {   
        /*  // FBO LAYOUT
        BULLETS:
        FBO SCENE is used to draw bullets first
        FBO Bright Pass uses the birght pass shader
        FBO Blur Pass uses gaussian
        FBO HOLD uses composite shader and adds the 2 bound textures and adds them together
        FBO HOLD uses blur pass and bright pass to get the bloom (without the actual orignial obj)

        use FBOHOLD again to add the original obj ontop of bloom

        GAME:
        FBO SCENE is used to draw rest objs
        BackBuffer (no FBO) uses composite shader to add FBO Hold and FBO Scene together for final image

        */

        // draw for bullets only
        fboScene.bindFrameBufferForDrawing();
        fboScene.clearFrameBuffer(glm::vec4(0.0f));

        for (int i = 0; i < p1Bullets.size(); i++)
        {
            if ( (P1.BulletBounceNum[i] < 3) && ( glm::length(P1.BulletMoveVal[i]) != 0) ) {
				if (toggleTexture == false)
					p1Bullets[i]->draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
				else
					p1Bullets[i]->draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
            }
            
            if ( (P2.BulletBounceNum[i] < 3 ) && (glm::length(P2.BulletMoveVal[i]) != 0) ){
				if (toggleTexture == false)
					p2Bullets[i]->draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
				else
					p2Bullets[i]->draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
            }

        }
        fboScene.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        fboScene.clearFrameBuffer(glm::vec4(0.0f));

        // bright Pass
        fboBrightPass.bindFrameBufferForDrawing();
        fboBrightPass.clearFrameBuffer(glm::vec4(0.0f));

            fboScene.bindTextureForSampling(0, GL_TEXTURE10);
            quad.drawParticle(bright, cameraTransform, cameraProjOrtho, pointLight);
            fboScene.unbindTexture(GL_TEXTURE10);

        fboBrightPass.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        fboBrightPass.clearFrameBuffer(glm::vec4(0.0f));

        // Blur pass
        fboBlur.bindFrameBufferForDrawing();
        fboBlur.clearFrameBuffer(glm::vec4(0.0f));
       
            fboBrightPass.bindTextureForSampling(0, GL_TEXTURE10);
            quad.drawParticle(gaussBlurr, cameraTransform, cameraProjOrtho, pointLight);
            fboBrightPass.unbindTexture(GL_TEXTURE10);

        fboBlur.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        fboBlur.clearFrameBuffer(glm::vec4(0.0f));

        
        // composite

        fboHold.bindFrameBufferForDrawing();
        fboHold.clearFrameBuffer(glm::vec4(0.0f));

            fboBrightPass.bindTextureForSampling(0, GL_TEXTURE10);
            fboBlur.bindTextureForSampling(0, GL_TEXTURE11);
            quad.drawParticle(composite, cameraTransform, cameraProjOrtho, pointLight);
            fboBlur.unbindTexture(GL_TEXTURE11);
            fboBrightPass.unbindTexture(GL_TEXTURE10);

                fboScene.bindTextureForSampling(0, GL_TEXTURE10);
                fboHold.bindTextureForSampling(0, GL_TEXTURE11);
                quad.drawParticle(composite, cameraTransform, cameraProjOrtho, pointLight);
                fboHold.unbindTexture(GL_TEXTURE11);
                fboScene.unbindTexture(GL_TEXTURE10);

        fboHold.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        fboHold.clearFrameBuffer(glm::vec4(0.0f));

        // draw rest of game on fbo
        fboScene.bindFrameBufferForDrawing();
        fboScene.clearFrameBuffer(glm::vec4(0.0f));

		if (toggleTexture == false)
		{
			arena.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			bars.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			slab.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			pillar1.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			pillar2.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			pillar3.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			if (freezeP2 == false)
				P1.Shield.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
			if (freezeP1 == false)
				P2.Shield.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
		}
		else
		{
			arena.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			bars.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			slab.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			pillar1.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			pillar2.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			pillar3.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			P1.Shield.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
			P2.Shield.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber);
		}

		if (toggleTexture == false)
		{
			if (freezeP2 == false)
				knight1Animation.draw(animationShader, cameraTransform, cameraProjection, pointLight, shaderNumber); //knight animation 1 walk
			if (freezeP1 == false)
				knight2Animation.draw(animationShader, cameraTransform, cameraProjection, pointLight, shaderNumber); //knight animation 2 walk
		}
		else
		{
			if (freezeP2 == false)
				knight1Animation.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber); //knight animation 1 walk
			if (freezeP1 == false)
				knight2Animation.draw(passthrough, cameraTransform, cameraProjection, pointLight, shaderNumber); //knight animation 2 walk
		}

        if ((P1.RoundWins == 4) || P2.RoundWins == 4) {
            if (se.stageSounds[2].startSound == false) {
                se.stageSounds[2].startSound = true;
                se.playSound(2, 0);
                se.stageSounds[2].channel->setMode(FMOD_LOOP_NORMAL);
            }
        }
        else {
            se.stageSounds[2].channel->setMode(FMOD_LOOP_OFF);
        }

     
        // MONKEY HEAD WIN CONDITION
        if (P1.RoundWins >= 5)
        {
           // monkeyHeadLeft.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
            gs = GameOver; se.fadeOutChannel(1, 0, 200000);
            P1.RoundWins = 0;
            P2.RoundWins = 0;
            PlayerOneWin = true;
            se.playSound(se.RandomInt(2, 4), 1); // victory lines
            se.stageSounds[2].startSound = false;
            se.stageSounds[2].channel->setMode(FMOD_LOOP_OFF);

        }
        if (P2.RoundWins >= 5)
        {
         //   monkeyHeadRight.draw(phong, cameraTransform, cameraProjection, pointLight, shaderNumber);
            gs = GameOver; se.fadeOutChannel(1, 0, 200000);
            P1.RoundWins = 0;
            P2.RoundWins = 0;
            PlayerTwoWin = true;
            se.playSound(se.RandomInt(2, 4), 2); // victory lines
            se.stageSounds[2].startSound = false;
            se.stageSounds[2].channel->setMode(FMOD_LOOP_OFF);
        }
        if (P2.RoundWins < P1IconsPos.size())
        {
            //For every point P2 Gets, P1 loses a life
            if(P2.RoundWins > 0)
                *P1IconsPos[P1IconsPos.size() - P2.RoundWins] = glm::vec3(10000);
//            for (int i = 0; i < P2.RoundWins; i++) {
//                P1Health.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);// do something
//            }
            P1HealthIcons.mesh->updateParticlePos(true, P1IconsPos);
            P1HealthIcons.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);
        }
        else
        {
            P1HealthIcons.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);
        }
        if (P1.RoundWins < P2IconsPos.size())
        {
            //For every point P1 Gets, P2 loses a life
            if (P1.RoundWins > 0)
                *P2IconsPos[P2IconsPos.size() - P1.RoundWins] = glm::vec3(10000);
            P2HealthIcons.mesh->updateParticlePos(true, P2IconsPos);
            P2HealthIcons.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);
        }
        else
        {
            P2HealthIcons.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);
        }
       
        partPosMeshAcess.mesh->updateParticlePos(true, particlePos);
        partPosMeshAcess.drawParticle(geo, cameraTransform, cameraProjection, pointLight);
      
        for (int i = 0; i < trailEmitterBase1.size(); i++) {
            if (P1.BulletBounceNum[i] < 3 && (glm::length(P1.BulletMoveVal[i]) > 0)) {
                trailEmitterBase1[i]->mesh->updateParticlePos(true, trailEmitterPosHOLDER[i]);
                trailEmitterBase1[i]->drawParticle(geo, cameraTransform, cameraProjection, pointLight);
            }
            if (P2.BulletBounceNum[i] < 3 && (glm::length(P2.BulletMoveVal[i]) > 0)) {
                trailEmitterBase2[i]->mesh->updateParticlePos(true, trailEmitterPos2HOLDER2[i]);
                trailEmitterBase2[i]->drawParticle(geo, cameraTransform, cameraProjection, pointLight);
            }
        }

        P1Health.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);
        P2Health.drawParticle(geo, cameraTransform, cameraProjOrtho, pointLight);

        fboScene.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        fboScene.clearFrameBuffer(glm::vec4(0.0f));

        // combine bullets fbo with game fbo

        fboScene.bindTextureForSampling(0, GL_TEXTURE10);
        fboHold.bindTextureForSampling(0, GL_TEXTURE11);

		if (shaderNumber.shaderNumber == 4)
			quad.drawParticle(postProcessing, cameraTransform, cameraProjOrtho, pointLight); // change it to another composite shader
		else
			quad.drawParticle(composite, cameraTransform, cameraProjOrtho, pointLight);
		
		fboHold.unbindTexture(GL_TEXTURE11);
        fboScene.unbindTexture(GL_TEXTURE10);



		//std::cout << shaderNumber.shaderNumber << std::endl;


    } // GAME MODE LOOP END
    else {

        fboScene.bindFrameBufferForDrawing();
        fboScene.clearFrameBuffer(glm::vec4(0.0f));

        if (gs == PressStart)
        {
            if ((int(totalGameTime) % 2) == 0)
                menuState[0]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
            else
                menuState[1]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
        }
        else if (gs == MainMenu)
        {
            if (playOrQuit == false)
                menuState[2]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
            else
                menuState[3]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
        }
        else if (gs == GameOver)
        {
            for (int i = 1; i < P1IconsOriginalPos.size(); i++) {
                *P1IconsPos[i] = P1IconsOriginalPos[i];
            }

            for (int i = 1; i < P2IconsOriginalPos.size(); i++) {
                *P2IconsPos[i] = P2IconsOriginalPos[i];
            }

            if (GameOverDelay > 4)
            {
                menuState[4]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
				credits.draw(phongUvScroll, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
				credits.uvScroll -= 0.001;
                PlayerOneWin = false;
                PlayerTwoWin = false;
            }
            else
            {
                if (PlayerOneWin)
                    menuState[5]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
                else if (PlayerTwoWin)
                    menuState[6]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
            }
        }
        else if (gs == loading) {
            menuState[7]->draw(UI, cameraTransform, cameraProjOrtho, pointLight, shaderNumber);
        }

        fboScene.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        fboScene.clearFrameBuffer(glm::vec4(0.0f));

        fboScene.bindTextureForSampling(0, GL_TEXTURE10);
        quad.drawParticle(passThroughParticles, cameraTransform, cameraProjOrtho, pointLight);
        fboScene.unbindTexture(GL_TEXTURE10);
        fboScene.unbindTexture(GL_TEXTURE0);
    }

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
    case 32: // spaceBar
        //System->playSound(sound2, nullptr, false, &channel2);
        se.playSound(0, 1);
        std::cout << "light constant   Attenuateion :" << pointLight.constantAttenuation   << std::endl;
        std::cout << "light linear     Attenuateion :" << pointLight.linearAttenuation     << std::endl;
        std::cout << "light quadtratic Attenuateion :" << pointLight.quadraticAttenuation  << std::endl;
        std::cout << "light postiion x              :" << pointLight.positionOrDirection.x << std::endl;
        std::cout << "light position y              :" << pointLight.positionOrDirection.y << std::endl;
        std::cout << "light position z              :" << pointLight.positionOrDirection.z << std::endl;

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
	case '1':
		shaderNumber.shaderNumber = 1;
		break;
	case '2':
		shaderNumber.shaderNumber = 2;
		break;
	case '3':
		shaderNumber.shaderNumber = 3;
		break;
	case '4':
		toggleTexture = !toggleTexture;
		break;
	case '5':
		shaderNumber.shaderNumber = 4;
		break;
	case '0':
		shaderNumber.shaderNumber = 0;
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
        P1.Death(&knight1, &knight2, p1Bullets, p2Bullets, P2.RoundWins, &P2);
        P2.Death(&knight1, &knight2, p1Bullets, p2Bullets, P1.RoundWins, &P1);
		P1.Shield.position = knight1.position;
		P1.Shield.translate = glm::translate(glm::mat4(), P1.Shield.position);
		P1.Shield.Radius = 1.4f;
		P2.Shield.position = knight2.position;
		P2.Shield.translate = glm::translate(glm::mat4(), P2.Shield.position);
		P2.Shield.Radius = 1.4f;
		knight1.currAngle = glm::pi<float>();
		knight2.currAngle = glm::pi<float>();
        P1.RoundWins = 0;
        P2.RoundWins = 0;
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
        std::cout << size2 + 1 << " : emmiters Found" << std::endl;
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
            {} // std::cout << line << std::endl;
            std::getline(myfile, line);
            if (line == "*")
            {} // std::cout << line << std::endl;

            loadedEmitters[loadedEmitters.size() - 1]->initialize(50);
        }
        myfile.close();
    }
    else std::cout << "Unable to open file";
}
