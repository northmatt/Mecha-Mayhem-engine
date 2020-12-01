#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Components/Player.h"
#include "Utilities/HitboxReader.h"
#include "Effects.h"

namespace Rendering
{
	void Update(entt::registry* reg, int numOfCams);

	extern glm::vec4 BackColour;
	extern glm::vec3 LightPos;
	extern glm::vec3 LightColour;
	extern glm::vec3 DefaultColour;

	extern HitboxGen* hitboxes;
	extern Effects* effects;
};

