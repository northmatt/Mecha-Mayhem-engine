#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Components/Player.h"
#include "Components/SpawnStation.h"
#include "Utilities/HitboxReader.h"
#include "Effects.h"

namespace Rendering
{
	void Update(entt::registry* reg, int numOfCams);

	extern glm::vec4 BackColour;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsPos;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsColour;
	extern size_t LightCount;
	extern glm::vec3 DefaultColour;

	extern HitboxGen* hitboxes;
	extern Effects* effects;
};

