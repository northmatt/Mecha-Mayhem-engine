#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Utilities/HitboxReader.h"

namespace Rendering
{
	void Update(entt::registry* reg, int numOfCams, float dt);

	extern glm::vec4 BackColour;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsPos;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsColour;
	extern size_t LightCount;
	extern glm::vec3 DefaultColour;

	extern HitboxGen* hitboxes;
};

