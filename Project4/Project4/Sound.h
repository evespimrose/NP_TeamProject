#pragma once
#include "inc/fmod.hpp"
#include <windows.h>
#pragma comment(lib,"fmod_vc.lib")

using namespace FMOD;

enum SOUND {
	BACKGROUND_SOUND,
	ATTACK_SOUND,
	CRUSH_SOUND,
	OVER_SOUND,
	DRIVE_SOUND
};

class SoundManager {
public:
	static SoundManager* instance;
	static SoundManager* sharedManager();
	void init();
	void loading();
	void play(int _type);
	void stop(int _cNum);
private:
	System* pFmod;
	Channel* ch[10];
	Sound* music[10];
	FMOD_RESULT r;
};
