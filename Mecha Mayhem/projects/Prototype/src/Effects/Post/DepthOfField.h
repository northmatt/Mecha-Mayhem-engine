#pragma once
#include "PostEffect.h"

class DepthOfFieldEffect : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;

	//Applies the effect to this buffer
	//passes the previous framebuffer with the texture to apply as parameter
	void ApplyEffect(PostEffect* buffer) override;

	void Reshape(unsigned width, unsigned height) override;

	void SetDepthLimit(float depth);
	float GetDepthLimit() const;

	void SetBlurPasses(int passes);
	int GetBlurPasses() const;

	void SetDrawBuffer(Framebuffer* buff) {
		_drawBuffer = buff;
	}

private:
	float _depth = 0.5f;
	int _blurPasses = 5;

	Framebuffer* _drawBuffer = nullptr;
};
