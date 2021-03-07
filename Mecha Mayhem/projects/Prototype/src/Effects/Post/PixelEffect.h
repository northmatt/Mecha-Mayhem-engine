#pragma once
#include "PostEffect.h"

class PixelEffect : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;

	//Applies the effect to this buffer
	//passes the previous framebuffer with the texture to apply as parameter
	void ApplyEffect(PostEffect* buffer) override;

	void Reshape(unsigned width, unsigned height) override;

	void SetPixelCount(int amt);
	int GetPixelCount() const;

private:
	int _pixelCount = 64.f;
};