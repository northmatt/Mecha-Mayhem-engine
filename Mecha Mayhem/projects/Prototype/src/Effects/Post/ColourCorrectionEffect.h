#pragma once

#include "PostEffect.h"
#include "Effects/LUT.h"

class ColourCorrection : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;

	//Applies the effect to this buffer
	//passes the previous framebuffer with the texture to apply as parameter
	void ApplyEffect(PostEffect* buffer) override;

	void Unload() override;

	//Getters
	float GetIntensity() const;
	LUT3D GetCube() const;

	//Setters
	void SetIntensity(float intensity);
	void SetCube(LUT3D &cube);

private:
	LUT3D *_cube;
	float _intensity = 1.f;
};