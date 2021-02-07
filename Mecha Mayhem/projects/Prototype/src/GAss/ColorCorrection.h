#pragma once

//#include "Graphics/Post/PostEffect.h"
//#include "Graphics/LUT.h"

#include "GAss/PostEffect.h"
#include "GAss/LUT.h"

class ColorCorrection : public PostEffect
{
public:
	//init framnebuffer
	//overrides post eff init
	void Init(unsigned width, unsigned height, LUT3D cube);

	void ApplyEffect(PostEffect* buffer) override;

	//apply
	//void DrawToScreen() override;


	float GetIntensity() const;

	void SetIntensity(float intensity);

	//void SetCube(LUT3D cube);

private:
	float _intensity = 1.f;
	LUT3D _cube;
};