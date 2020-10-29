#pragma once
#include "ECS.h"
#include "BackEndThings/BackEnd.h"
#include "ObjLoader.h"


namespace Rendering
{
	void Update(entt::registry* reg);

	static glm::vec4 BackColour{ 0.2f, 0.2f, 0.2f, 1.f };
};

