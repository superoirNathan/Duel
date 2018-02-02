#pragma once
#include <FMOD/fmod.hpp>
#include <string>
#include <unordered_map>

class SoundSystem
{
public:
	static SoundSystem* getInstance();
	void createSound(std::string);
	void playSound(std::string, bool = false);
	void releaseSound(std::string);
	void update();
	~SoundSystem();

private:
	std::unordered_map<std::string, FMOD::Sound*> m_sounds;
	static SoundSystem* m_instance;
	FMOD::Channel* m_channel;
	FMOD::System* m_system;
	SoundSystem();
};
