#pragma once
#include <string>
#include <vector>

#include "glm/common.hpp"

#include "fmod.hpp"
#include "fmod_errors.h"

//studio
#include "fmod_studio.hpp"
#include "fmod_studio_common.h"


//thank you GPC team for helping us set up this system
class SoundManager {
public:

	/*
	initializes the sound manager
	defaultFilePath is the default path to a sound, for example ./Assets/Sounds
	numChannels is the number of channels you want to allocate to FMOD core
	*/
	static void init(const std::string& defaultFilePath,unsigned numChannels = 512);
	
	/*
	A setter for the default path
	*/
	static void setDefaultPath(const std::string& defaultFilePath) { _soundPath = defaultFilePath; }
	
	/*
	updates the sound manager, should be called every frame
	*/
	static void update();

	/*
	loads a 2D sound with the relative file path
	returns the index at which the sound is stored in the sounds vector
	*/
	static unsigned load2DSound(const std::string& relativeFilePath);

	/*
	creates a channel group, returns the index at which it was created
	*/
	static unsigned createChannelGroup(const std::string& groupName);

	/*
	plays a 2d sound at the respective indicies
	*/
	static unsigned play2DSound(unsigned soundIndex, unsigned groupIndex);

	//stop a sound
	static void stop2DSound(unsigned channelIndex);
	//stop a group
	static void stopChannelGroup(unsigned groupIndex);
	//stops everything
	static void stopEverything();
	//sets channel volume level
	static void setVolume(unsigned index, float percent);
	//sets channel group volume level
	static void setGroupVolume(unsigned index, float percent);
	//sets channel pitch pitch level
	static void setGroupPitch(unsigned index, float percent);
	//limit how many sounds playing in a group
	static void limitGroups(unsigned limit);

	static void PauseEverything();
	static void PlayEverything();

	//is channel playing
	static bool isChannelPlaying(unsigned index);
	//is group playing
	static bool isGroupPlaying(unsigned index);

	//set index to loop n times
	static void loopSound(unsigned index, unsigned loopCount);

	/*
	a function used to check the result of an FMOD opertation
	whereError is a parameter used to described where the error occured,
	so that when its printed in console you know where to look
	*/
	static void checkFmodErrors(FMOD_RESULT& result, const std::string& whereError);

	//The FMOD system, the main thing that does a lot of the work for you
	static FMOD::System* _system;

	static std::vector<FMOD::Sound*> _sounds;
	static std::vector<FMOD::ChannelGroup*>_channelGroups;
	static std::vector<FMOD::Channel*>_channels;
private:
	static std::string _soundPath;
	static bool _initialized;
	static FMOD_RESULT _result;
};

class Sound2D {
public:
	Sound2D() {}
	Sound2D(const std::string& path, const std::string& groupName);

	void play();
	void stop();
	void loop();
	void stopGroup();
	void setVolume(float percent);
	void setGroupVolume(float percent);
	void setGroupPitch(float percent);
	float getGroupPitch();

	void setSoundHandle(unsigned soundHandle) { _sound = soundHandle; }
	void setGroupHandle(unsigned groupHandle) { _group = groupHandle; }

	unsigned getSoundHandle() const { return _sound; }
	unsigned getGroupHandle() const { return _group; }

	bool isPlaying();
	bool isGroupPlaying();
	void setLoopCount(unsigned count);

private:
	unsigned _sound = 0, _group = 0, _channel = 0;
};