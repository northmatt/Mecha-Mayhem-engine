#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Components/Player.h"
#include "Components/SpawnStation.h"
#include "Utilities/HitboxReader.h"
#include "Effects.h"

namespace Rendering
{
	//sets up the VP
	void Init(int width, int height);

	void Update(entt::registry* reg, int numOfCams, bool paused);
	void DrawPauseScreen(Sprite image);

	extern glm::vec4 BackColour;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsPos;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsColour;
	extern size_t LightCount;
	extern glm::vec3 DefaultColour;
	extern glm::vec3 AmbientColour;
	extern glm::float32 AmbientStrength;

	extern HitboxGen* hitboxes;
	extern Effects* effects;
	extern Camera orthoVP;
};

