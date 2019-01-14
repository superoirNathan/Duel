#pragma once
//template from Brent Cowan Jan.22, 2018
//Nathan Alphonse 100523109

//This pre-processor definition add the lib file in code instead of using the project settings.
//This makes adding your code to other projects easier, but it may not work on other operating
//systems or compilers.
#pragma comment (lib, "../lib/fmod_vc.lib")

#include <Windows.h>
#include <math.h>
#include "FMOD\fmod.hpp"
#include "FMOD\fmod_errors.h"
#include <iostream> //cin & cout
#include <conio.h> //Keyboard input
#include <vector>
using namespace std;

void FmodErrorCheck(FMOD_RESULT result);

struct Listener
{
	FMOD_VECTOR pos = { 0.0f, 0.0f, -1.0f };
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
};
struct SoundFile
{
    // channel = taken from sound pointer first 
    FMOD::Channel *channel = NULL;
    // sound = default loader (1 per noise)
    FMOD::Sound *sound = NULL;
    // sound pos needed for 3D, vel needed for doppler effects
    FMOD_VECTOR soundPos = {0.f,0.f,0.f};
    FMOD_VECTOR soundVel = {0.f, 0.f, 0.f};
    // set paused when not in use
    bool setPaused = true;
    std::string name = "empty";
    // bools to know when channel starts and ends
    bool startSound = false;
    bool soundEnded = false;
};

class SoundEngine
{
private:
	bool init;
	FMOD_RESULT TEST;

public:
	SoundEngine();
	~SoundEngine();
	bool Init();
	void CleanUp();
    void update(float dt = 0.0f);
    float Random(float a, float b);
    int RandomInt(int a, int b);

    void setListener(FMOD_VECTOR otherPos, FMOD_VECTOR otherForward, FMOD_VECTOR otherUp, FMOD_VECTOR otherVel);
    bool loadSound(std::string wavFile, bool loop = false, float min = 1.f, float max = 30);
    void playSound( int channelNumb, int selectSF = 0);
    void loopOnOff(int channelNumb, bool onOff = false, int selectSF = 0);
    void fadeOutChannel(int channelNumb, int selectSF = 0, float offSet = 30000);
    void fadeinChannel(int channelNumb, int selectSF = 0, float offSet = 60000);

    void addChannel(std::string name, FMOD::Sound* sound = NULL, FMOD_VECTOR soundPos = { 0.f,0.f,0.f }, FMOD_VECTOR soundVel = { 0.f, 0.f, 0.f }, int selectSF = 0);
    //Sound File
	FMOD::Sound *sound = NULL;
	//Sound Channel
	FMOD::Channel *channel;
    //Create a pitch shift effect
    FMOD::DSP *pitchShift;
    
    std::vector<SoundFile> stageSounds; // selectSF 0
    std::vector<SoundFile> redKnight;   // selectSF 1
    std::vector<SoundFile> blueKnight;  // selectSF 2

    bool fileExists = false;

	FMOD::System    *system;
	unsigned int     version;
	void            *extradriverdata = 0;
	Listener         listener;
	FMOD_RESULT      result;
};