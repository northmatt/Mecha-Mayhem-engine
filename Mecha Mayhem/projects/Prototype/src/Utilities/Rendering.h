#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Utilities/HitboxReader.h"

namespace Rendering
{
	void Update(entt::registry* reg, int numOfCams, float dt);

	extern glm::vec4 BackColour;
	extern glm::vec3 LightPos[MAX_LIGHTS];
	extern glm::vec3 LightColour[MAX_LIGHTS];
	extern size_t LightCount;
	extern glm::vec3 DefaultColour;

	extern HitboxGen* hitboxes;
};

