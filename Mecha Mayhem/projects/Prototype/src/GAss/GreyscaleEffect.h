#pragma once

#include "Graphics/Post/PostEffect.h"

class GreyscaleEffect : public PostEffect
{
public:
	//init framnebuffer
	//overrides post eff init
	void Init(unsigned width, unsigned height) override;

	void ApplyEffect(PostEffect* buffer) override;

	//apply
	//void DrawToScreen() override;


	float GetIntensity() const;

	void SetIntensity(float intensity);

private:
	float _intensity = 1.f;
};