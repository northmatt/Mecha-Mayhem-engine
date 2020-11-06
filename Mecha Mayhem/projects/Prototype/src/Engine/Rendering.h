#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Components/ObjLoader.h"

namespace Rendering
{
	void Update(entt::registry* reg);

	extern glm::vec4 BackColour;
	extern glm::vec3 LightPos;
	extern glm::vec3 LightColour;
	extern glm::vec3 DefaultColour;
};

