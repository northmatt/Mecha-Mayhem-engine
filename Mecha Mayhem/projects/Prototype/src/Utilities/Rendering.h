#pragma once
#include "Engine/BackEnd.h"
#include "Components/ECS.h"
#include "Components/Player.h"
#include "Utilities/HitboxReader.h"
#include "Effects.h"
#include "Effects/FrameEffects.h"

namespace Rendering
{
	//sets up the VP
	void Init(int width, int height);

	void Update(entt::registry* reg, int numOfCams, bool paused);
	//void DrawPauseScreen(Sprite image);

	//doesn't bind the buffer
	void RenderForShading(entt::registry* reg);

	extern glm::vec3 BackColour;
	extern glm::vec3 DefaultColour;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsPos;
	extern std::array<glm::vec3, MAX_LIGHTS> LightsColour;
	extern size_t LightCount;

	extern HitboxGen* hitboxes;
	extern Effects* effects;
	extern FrameEffects* frameEffects;
	extern Camera orthoVP;
};

