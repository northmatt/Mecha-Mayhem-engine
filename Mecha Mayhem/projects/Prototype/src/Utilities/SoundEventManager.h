#pragma once
#include <AudioEngine.h>
#include "BLM.h"

class SoundEventManager abstract
{
public:
	enum class SOUND {
		MAINMENU,
		PICKUP,
		RELOAD,
		SWAP,
		STEP,
		SHOOT,
		PUNCH,
		HIT,
		GAMEMUSIC,
		MATCHEND
	};

	static void Init() {
		engine = &AudioEngine::Instance();
		engine->Init();
		engine->LoadBankWithString("Master");
		engine->LoadBus("Music", "bus:/Music");
		engine->LoadBus("SFX", "bus:/SFX");

		//						number of sound effects
		soundEventQueues.reserve(9);
		//deal with all the options		evnt id followed by how many instances
		soundEventQueues.push_back({ "Main Menu", 1 });		//	SOUND::MAINMENU
		soundEventQueues.push_back({ "pickup", 5 });		//	SOUND::PICKUP
		soundEventQueues.push_back({ "reload", 5 });		//	SOUND::RELOAD
		soundEventQueues.push_back({ "swap", 5 });			//	SOUND::SWAP
		soundEventQueues.push_back({ "steps", 20 });		//	SOUND::STEP
		soundEventQueues.push_back({ "Pew", 30 });			//	SOUND::SHOOT
		soundEventQueues.push_back({ "punch", 4 });			//	SOUND::PUNCH
		soundEventQueues.push_back({ "hit", 20 });			//	SOUND::HIT
		soundEventQueues.push_back({ "Game Music", 1 });	//	SOUND::GAMEMUSIC
		soundEventQueues.push_back({ "Game Over", 1 });		//	SOUND::MATCHEND


		engine->GetListener().SetUp(BLM::GLMup);
		//sets the starting positions
		ResetEvents();
	}

	static void Update() {
		engine->Update();
	}

	static void Stop() {
		soundEventQueues.clear();
		engine->Shutdown();
	}

	static void StopSounds() {
		engine->StopAllSounds();
	}

	//updates listener position
	static void UpdatePosition(const glm::vec3& pos = BLM::GLMzero, const glm::vec3& forwards = glm::vec3(0, 0, -1)) {
		engine->GetListener().SetPosition(pos);
		engine->GetListener().SetForward(forwards);
		if (ThreeDSounds)
		for (int i(0); i < soundEventQueues.size(); ++i) {
			auto& q = soundEventQueues[i];
			q.back().SetPosition(q._pos);
		}
	}

	//make all events at 0
	static void ResetEvents() {
		for (int i(0); i < soundEventQueues.size(); ++i) {
			auto& q = soundEventQueues[i];
			for (int j(0); j < q.size(); ++j) {
				q.pushpop().SetPosition(BLM::GLMzero);
			}
		}
		UpdatePosition();
		StopSounds();
	}

	//get a new events of the specified sound
	static AudioEvent& Create(SOUND sound) {
		return soundEventQueues[int(sound)].pushpop();
	}

	//plays one of the events of the specified sound
	static void Play(SOUND sound, const glm::vec3& pos = BLM::GLMzero) {
		AudioEvent& soundEvent = soundEventQueues[int(sound)].pushpop();
		if (ThreeDSounds) {
			soundEvent.SetPosition(soundEventQueues[int(sound)]._pos = pos);
		}
		soundEvent.Restart();
	}

	//doesn't work lol
	static bool ThreeDSounds;

private:
	static AudioEngine* engine;

	class SoundQueue
	{
	public:
		SoundQueue(const std::string& name, size_t size)
			: _size(size)
		{
			_events.reserve(size);
			for (int i(0); i < size; ++i) {
				_events.push_back(&(engine->CreateSoundEvent(name + " " + std::to_string(i), "event:/" + name)));
				//_events.push_back(name + ' ' + std::to_string(i));
				//engine->CreateSoundEvent(_events[i], "event:/" + name);
			}
		}

		//does the equivalent of a push(pop()) in this limited system
		//basically cycles through sounds (queue is always full)
		AudioEvent& pushpop() {
			_back = _front;
			_front = (_front + 1) % _size;
			return *_events[_back];
			//return engine->GetEvent(_events[_back]);
		}

		/*	these aren't used because we dont need these functionalities
		
		//pops the front and returns what it poped (not deleted from array)
		AudioEvent& pop() {
			int index = _front;
			_front = (_front + 1) % _size;
			return *_events[index];
		}

		//pushes to the back and returns what was pushed
		AudioEvent& push(AudioEvent& item) {
			_back = (_back + 1) % _size;
			if (_back == _front)
				throw std::runtime_error("queue full\n");
			return *(_events[_back] = &item);
		}
		*/

		//last pushed events
		AudioEvent& back() {	return *_events[_back];		}
		//AudioEvent& back() {	return engine->GetEvent(_events[_back]);		}

		//next pushed events
		AudioEvent& front() {	return *_events[_front];	}
		//AudioEvent& front() {	return engine->GetEvent(_events[_front]);	}

		size_t size() { return _size; }

		glm::vec3 _pos = BLM::GLMzero;
	private:
		int _front = 0;
		int _back = 0;
		const size_t _size;
		std::vector<AudioEvent*> _events = {};
		//std::vector<std::string> _events = {};
	};

	static std::vector<SoundQueue> soundEventQueues;
};

inline bool SoundEventManager::ThreeDSounds = false;
inline AudioEngine* SoundEventManager::engine = nullptr;
inline std::vector<SoundEventManager::SoundQueue> SoundEventManager::soundEventQueues;