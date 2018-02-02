#include "SoundSystem.h"

SoundSystem* SoundSystem::m_instance;

SoundSystem::SoundSystem()
{
	FMOD::System *System;
	if (FMOD::System_Create(&System) != FMOD_OK)
		throw;

	int driverCount = 0;
	System->getNumDrivers(&driverCount);
	if (driverCount == 0)
		throw;

	System->init(32, FMOD_INIT_NORMAL, nullptr);
}

SoundSystem * SoundSystem::getInstance()
{
	if (!m_instance)
		m_instance = new SoundSystem();
	return m_instance;
}

void SoundSystem::createSound(std::string name)
{
	FMOD::Sound *sound = nullptr;
	std::string path = "Sounds/" + name + ".mp3";
	m_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
	m_sounds[name] = sound;
}

void SoundSystem::playSound(std::string soundName, bool loop)
{
	if (!loop)
		m_sounds[soundName]->setMode(FMOD_LOOP_OFF);
	else {
		m_sounds[soundName]->setMode(FMOD_LOOP_NORMAL);
		m_sounds[soundName]->setLoopCount(-1);
	}
	m_system->playSound(m_sounds[soundName], nullptr, false, &m_channel);
}

void SoundSystem::releaseSound(std::string soundName)
{
	m_sounds[soundName]->release();
}

void SoundSystem::update()
{
	m_system->update();
}

SoundSystem::~SoundSystem()
{
}
