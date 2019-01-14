//template from Brent Cowan Jan. 15, 2018
//Nathan Alphonse 100523109

#include "SoundEngine.h"

void FmodErrorCheck(FMOD_RESULT TEST)
{
    if (TEST != FMOD_OK)
    {
        cout << "Fmod Error: " << FMOD_ErrorString(TEST) << endl;
        std::system("pause");
    }
}

SoundEngine::SoundEngine()
    :init(false)
{
}

SoundEngine::~SoundEngine()
{
	CleanUp();
}

void SoundEngine::CleanUp()
{
//    if (sound != NULL) {
//        result = sound->release(); FmodErrorCheck(result);
//    }
  
    for (int i = 0; i < stageSounds.size(); i++) {
        if (stageSounds[i].sound != NULL && stageSounds[i].sound != sound)
        result = stageSounds[i].sound->release();
        FmodErrorCheck(result);
    }
    for (int i = 0; i < redKnight.size(); i++) {
        if (redKnight[i].sound != NULL && redKnight[i].sound != sound)
            result = redKnight[i].sound->release();
        FmodErrorCheck(result);
    }
    for (int i = 0; i < blueKnight.size(); i++) {
        if (blueKnight[i].sound != NULL && blueKnight[i].sound != sound)
            result = blueKnight[i].sound->release();
        FmodErrorCheck(result);
    }

	TEST = system->close();
	FmodErrorCheck(TEST);
	TEST = system->release();
	FmodErrorCheck(TEST);

	init = false;
}

bool SoundEngine::Init()
{
	if (!init)
	{
		init = true;

		TEST = FMOD::System_Create(&system);
		FmodErrorCheck(TEST);
		if (TEST != FMOD_OK) { init = false; }

		TEST = system->getVersion(&version);
		if (TEST != FMOD_OK) { init = false; }
		FmodErrorCheck(TEST);

		TEST = system->init(100, FMOD_INIT_NORMAL, extradriverdata);
		FmodErrorCheck(TEST);
		if (TEST != FMOD_OK) { init = false; }

		/*
		Set the distance units. (meters/feet etc).
		*/
		TEST = system->set3DSettings(1.0, 1.0f, 1.0f);
		FmodErrorCheck(TEST);
		if (TEST != FMOD_OK) { init = false; }

        sound = NULL;
        channel = NULL;
        system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShift);
        pitchShift->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.5f);
	}

	return init;
}


void SoundEngine::setListener(FMOD_VECTOR otherPos, FMOD_VECTOR otherForward, FMOD_VECTOR otherUp, FMOD_VECTOR otherVel) {

        //Place the listener
        listener.pos = otherPos; listener.forward = otherForward;
        listener.up = otherUp; listener.vel = otherVel;

        result = system->set3DListenerAttributes(0, &listener.pos, &listener.vel, &listener.forward, &listener.up);

        FmodErrorCheck(result); 
}

bool SoundEngine::loadSound(std::string wavFile, bool loop, float min, float max) {

    //Load a se.sound
    result = system->createSound( wavFile.c_str(), FMOD_3D, 0, &sound);
    FmodErrorCheck(result);
    if (result != FMOD_OK) {
        fileExists = false;
    }
    else
        fileExists = true;
    std::cout << result << std::endl;
    result = sound->set3DMinMaxDistance(min, max);
    FmodErrorCheck(result);
    if (loop == false)
        result = sound->setMode(FMOD_LOOP_OFF);
    else
        result = sound->setMode(FMOD_LOOP_NORMAL); 

    FmodErrorCheck(result);

    return fileExists;
}

void SoundEngine::playSound(int channelNumb, int selectSF)
{
    if (selectSF == 0) {
        if (stageSounds.size() != 0) {
            //Play se.sound
            result = system->playSound(stageSounds[channelNumb].sound, 0, true, &stageSounds[channelNumb].channel); FmodErrorCheck(result);
            result = stageSounds[channelNumb].channel->set3DAttributes(&stageSounds[channelNumb].soundPos, &stageSounds[channelNumb].soundVel);    FmodErrorCheck(result);
            result = stageSounds[channelNumb].channel->setPaused(false);   FmodErrorCheck(result);
            result = stageSounds[channelNumb].channel->setMode(FMOD_3D_LINEARROLLOFF);   FmodErrorCheck(result);
            stageSounds[channelNumb].startSound = true;
            stageSounds[channelNumb].soundEnded = false;

        }
        else
            std::cout << "stageSounds channel array size is 0" << std::endl;
    }
    else     if (selectSF == 1) {
        if (redKnight.size() != 0) {
            //Play se.sound
            result = system->playSound(redKnight[channelNumb].sound, 0, true, &redKnight[channelNumb].channel); FmodErrorCheck(result);
            result = redKnight[channelNumb].channel->set3DAttributes(&redKnight[channelNumb].soundPos, &redKnight[channelNumb].soundVel);    FmodErrorCheck(result);
            result = redKnight[channelNumb].channel->setPaused(false);   FmodErrorCheck(result);
            result = redKnight[channelNumb].channel->setMode(FMOD_3D_LINEARROLLOFF);   FmodErrorCheck(result);
            redKnight[channelNumb].startSound = true;
            redKnight[channelNumb].soundEnded = false;
        }
        else
            std::cout << "redKnight channel array size is 0" << std::endl;
    }
    else     if (selectSF == 2) {
        if (blueKnight.size() != 0) {
            //Play se.sound
            result = system->playSound(blueKnight[channelNumb].sound, 0, true, &blueKnight[channelNumb].channel); FmodErrorCheck(result);
            result = blueKnight[channelNumb].channel->set3DAttributes(&blueKnight[channelNumb].soundPos, &blueKnight[channelNumb].soundVel);    FmodErrorCheck(result);
            result = blueKnight[channelNumb].channel->setPaused(false);   FmodErrorCheck(result);
            result = blueKnight[channelNumb].channel->setMode(FMOD_3D_LINEARROLLOFF);   FmodErrorCheck(result);
            blueKnight[channelNumb].startSound = true;
            blueKnight[channelNumb].soundEnded = false;
        }
        else
            std::cout << "blue Knight channel array size is 0" << std::endl;
    }
}
void SoundEngine::loopOnOff(int channelNumb, bool onOff, int selectSF) {
    if (selectSF == 0) {
        if (onOff == true)
            result = stageSounds[channelNumb].channel->setMode(FMOD_LOOP_NORMAL); //    result = sound->setMode(FMOD_LOOP_NORMAL);
        else
            result = stageSounds[channelNumb].channel->setMode(FMOD_LOOP_OFF);
    }
    else     if (selectSF == 1) {
        if (onOff == true)
            result = redKnight[channelNumb].channel->setMode(FMOD_LOOP_NORMAL); //    result = sound->setMode(FMOD_LOOP_NORMAL);
        else
            result = redKnight[channelNumb].channel->setMode(FMOD_LOOP_OFF);
    }
    else     if (selectSF == 2) {
        if (onOff == true)
            result = blueKnight[channelNumb].channel->setMode(FMOD_LOOP_NORMAL); //    result = sound->setMode(FMOD_LOOP_NORMAL);
        else
            result = blueKnight[channelNumb].channel->setMode(FMOD_LOOP_OFF);
    }
}

void SoundEngine::fadeOutChannel(int channelNumb, int selectSF, float offSet) {
    //Create a fade effect
    unsigned long long parentclock;
    if (selectSF == 0) {
        result = stageSounds[channelNumb].channel->getDSPClock(NULL, &parentclock);
        result = stageSounds[channelNumb].channel->addFadePoint(parentclock, 1.0f);
        result = stageSounds[channelNumb].channel->addFadePoint(parentclock + offSet, 0.0f);
    }
    else if (selectSF == 1) {
        result = redKnight[channelNumb].channel->getDSPClock(NULL, &parentclock);
        result = redKnight[channelNumb].channel->addFadePoint(parentclock, 1.0f);
        result = redKnight[channelNumb].channel->addFadePoint(parentclock + offSet, 0.0f);
    }
    else if (selectSF == 2) {
        result = blueKnight[channelNumb].channel->getDSPClock(NULL, &parentclock);
        result = blueKnight[channelNumb].channel->addFadePoint(parentclock, 1.0f);
        result = blueKnight[channelNumb].channel->addFadePoint(parentclock + offSet, 0.0f);
    }
}
void SoundEngine::fadeinChannel(int channelNumb, int selectSF, float offSet) {
    //Create a fade effect
    unsigned long long parentclock;
    if (selectSF == 0) {
        result = stageSounds[channelNumb].channel->getDSPClock(NULL, &parentclock);
        result = stageSounds[channelNumb].channel->addFadePoint(parentclock, 0.0f);
        result = stageSounds[channelNumb].channel->addFadePoint(parentclock + offSet, 1.0f);
    }
    else if (selectSF == 1) {
        result = redKnight[channelNumb].channel->getDSPClock(NULL, &parentclock);
        result = redKnight[channelNumb].channel->addFadePoint(parentclock, 0.0f);
        result = redKnight[channelNumb].channel->addFadePoint(parentclock + offSet, 1.0f);
    }
    else if (selectSF == 2) {
        result = blueKnight[channelNumb].channel->getDSPClock(NULL, &parentclock);
        result = blueKnight[channelNumb].channel->addFadePoint(parentclock, 0.0f);
        result = blueKnight[channelNumb].channel->addFadePoint(parentclock + offSet, 1.0f);
    }

}


void SoundEngine::addChannel(std::string name, FMOD::Sound* sound, FMOD_VECTOR soundPos, FMOD_VECTOR soundVel, int selectSF){
    if (selectSF == 0) {
        stageSounds.emplace_back(SoundFile{ 0, sound, soundPos, soundVel, true, name , false, false}); // emplace_back is basically push_back
    }
    else if (selectSF == 1) {
        redKnight.emplace_back(SoundFile{ 0, sound, soundPos, soundVel, true, name, false, false }); 
    }
    else if (selectSF == 2) {
        blueKnight.emplace_back(SoundFile{ 0, sound, soundPos, soundVel, true, name, false, false }); 
    }
}

void SoundEngine::update(float dt) {

    //Move the se.sound
    if (stageSounds.size() != 0) //If the channel array is not empty
    {
        //move all sounds per distance = velocity * time
        for (int i = 0; i < stageSounds.size(); i++) {

            stageSounds[i].channel->set3DAttributes(&stageSounds[i].soundPos, &stageSounds[i].soundVel);
        }
    }


    //Move the se.sound
    if (redKnight.size() != 0) //If the channel array is not empty
    {
        for (int i = 0; i < redKnight.size(); i++) {
         
            redKnight[i].channel->set3DAttributes(&redKnight[i].soundPos, &redKnight[i].soundVel);
        }
    }

    //Move the se.sound
    if (blueKnight.size() != 0) //If the channel array is not empty
    {
        for (int i = 0; i < blueKnight.size(); i++) {
          
            blueKnight[i].channel->set3DAttributes(&blueKnight[i].soundPos, &blueKnight[i].soundVel);
            
//            bool isOn = false;
//            blueKnight[i].channel->isPlaying(&isOn);
//            if (!isOn) {
//                blueKnight[i].startSound = false;
//                blueKnight[i].soundEnded = true;
//            }
        }
    }
    system->update(); //Must call this every frame
}

//A function to generate a random number between a and b
float SoundEngine::Random(float a, float b)
{
    return (float(rand()) / float(RAND_MAX)) * abs(b - a) + min(a, b);
}
//A function to generate a random number between a and b
int SoundEngine::RandomInt(int a, int b)
{
    int temp = a + (rand() % static_cast<int>(b - a + 1)); //
        std::cout << "  rand->  "<<temp << std::endl;
        return temp;
}
