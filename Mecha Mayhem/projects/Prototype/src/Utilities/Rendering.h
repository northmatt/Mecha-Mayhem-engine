#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Utilities/HitboxReader.h"

namespace Rendering
{
	void Update(entt::registry* reg, int numOfCams);

	extern glm::vec4 BackColour;
	extern glm::vec3 LightPos;
	extern glm::vec3 LightColour;
	extern glm::vec3 DefaultColour;

	extern HitboxGen* hitboxes;
};

